// Copyright 2025 ayumax. All Rights Reserved.
#ifdef PLATFORM_WINDOWS
#include "WindowCaptureSession.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <windows.graphics.directx.direct3d11.interop.h>
#include <windows.graphics.capture.interop.h>
#include <stdexcept>
#include <winrt/Windows.Foundation.h>
#include "HAL/RunnableThread.h"
#include "WindowCapture2DMacros.h"

inline winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice CreateDirect3DDevice(ID3D11Device* d3dDevice)
{
	winrt::com_ptr<IDXGIDevice> dxgiDevice;
	winrt::check_hresult(d3dDevice->QueryInterface(__uuidof(IDXGIDevice), dxgiDevice.put_void()));
	winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device = nullptr;
	winrt::check_hresult(
		CreateDirect3D11DeviceFromDXGIDevice(dxgiDevice.get(),
		                                     reinterpret_cast<IInspectable**>(winrt::put_abi(device))));
	return device;
}

WindowCaptureSession::WindowCaptureSession()
	: m_hwnd(nullptr), m_state(CaptureState::Stopped)
{
}

WindowCaptureSession::~WindowCaptureSession()
{
	Stop();
}

bool WindowCaptureSession::HasNewFrame() const
{
	return m_state == CaptureState::Running && m_hasNewFrame.load();
}

bool WindowCaptureSession::GetFrameInfo(WCFrameDesc* OutDesc) const
{
	if (m_captureItem)
	{
		auto size = m_captureItem.Size();
		OutDesc->width = size.Width;
		OutDesc->height = size.Height;
		OutDesc->stride = size.Width * 4;
		OutDesc->bytesPerPixel = 4;
	}
	else
	{
		OutDesc->width = 0;
		OutDesc->height = 0;
		OutDesc->stride = 0;
		OutDesc->bytesPerPixel = 0;
	}

	return true;
}

void WindowCaptureSession::UseBuffer(const TFunction<void()>& Action)
{
	FScopeLock lock(&m_mutex_buffer);
	Action();
}

void WindowCaptureSession::InitializeCaptureResources()
{
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};
	D3D_FEATURE_LEVEL featureLevel;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = D3D11CreateDevice(
		nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags,
		featureLevels, ARRAYSIZE(featureLevels),
		D3D11_SDK_VERSION, &device, &featureLevel, &context);

	if (FAILED(hr))
	{
		WC_LOG(Error, TEXT("Failed to create D3D11 device"));
	}

	m_d3dDevice = device;
	m_d3dContext = context;
}

void WindowCaptureSession::InitializeWinRTCaptureResources()
{
	if (!m_hwnd)
	{
		throw std::runtime_error("HWND is null. Cannot initialize WinRT resources.");
	}

	winrt::Windows::Foundation::IActivationFactory factory =
		winrt::get_activation_factory<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>();
	winrt::com_ptr<IGraphicsCaptureItemInterop> interop = factory.as<IGraphicsCaptureItemInterop>();
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem item{nullptr};
	winrt::check_hresult
	(
		interop->CreateForWindow
		(
			m_hwnd,
			winrt::guid_of<winrt::Windows::Graphics::Capture::GraphicsCaptureItem>(),
			winrt::put_abi(item)
		)
	);

	const auto d3dDevice = CreateDirect3DDevice(m_d3dDevice.Get());

	const auto framePool = winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::Create(
		d3dDevice,
		winrt::Windows::Graphics::DirectX::DirectXPixelFormat::B8G8R8A8UIntNormalized,
		2,
		item.Size()
	);

	const auto session = framePool.CreateCaptureSession(item);

	m_captureItem = item;
	m_framePool = framePool;
	m_session = session;

	m_session.StartCapture();
}

FWCWorkerThread::EWorkState WindowCaptureSession::CaptureWork()
{
	if (m_state != CaptureState::Running) { return FWCWorkerThread::EWorkState::Stop; }

	FScopeLock lockThread(&m_mutex_thread);
	
	try
	{
		auto frame = m_framePool.TryGetNextFrame();
		if (!frame) { return FWCWorkerThread::EWorkState::Wait; }

		auto surface = frame.Surface();

		winrt::impl::com_ref<::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess> dxgiAccess =
			surface.as<::Windows::Graphics::DirectX::Direct3D11::IDirect3DDxgiInterfaceAccess>();
		Microsoft::WRL::ComPtr<ID3D11Texture2D> srcTexture;
		winrt::check_hresult(dxgiAccess->GetInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(srcTexture.GetAddressOf())));
		D3D11_TEXTURE2D_DESC desc = {};

		srcTexture->GetDesc(&desc);

		if (m_captureItem)
		{
			auto itemSize = m_captureItem.Size();
			if (itemSize.Width != static_cast<int>(desc.Width) || itemSize.Height != static_cast<int>(desc.Height))
			{
				m_frameArrivedRevoker.revoke();
				m_framePool.Close();
				m_session.Close();
				InitializeWinRTCaptureResources();
				return FWCWorkerThread::EWorkState::Wait;
			}
		}

		if (!m_stagingTexture || m_stagingDesc.Width != desc.Width || m_stagingDesc.Height != desc.Height)
		{
			desc.Usage = D3D11_USAGE_STAGING;
			desc.BindFlags = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.MiscFlags = 0;
			m_stagingTexture.Reset();
			m_d3dDevice->CreateTexture2D(&desc, nullptr, &m_stagingTexture);
			m_stagingDesc = desc;
		}

		m_d3dContext->CopyResource(m_stagingTexture.Get(), srcTexture.Get());
		D3D11_MAPPED_SUBRESOURCE mapped = {};

		m_d3dContext->Map(m_stagingTexture.Get(), 0, D3D11_MAP_READ, 0, &mapped);
		UINT rowPitch = desc.Width * 4;
		UINT bufferSize = rowPitch * desc.Height;

		{
			FScopeLock lock(&m_mutex_buffer);
			if (m_state != CaptureState::Running) { return FWCWorkerThread::EWorkState::Stop; }

			if (m_buffer.Num() != bufferSize)
			{
				m_buffer.SetNum(bufferSize, EAllowShrinking::Yes);
			}

			UINT copyBytesPerLine = desc.Width * 4;
			for (UINT y = 0; y < desc.Height; ++y)
			{
				memcpy(
					m_buffer.GetData() + (desc.Height - 1 - y) * rowPitch,
					(BYTE*)mapped.pData + y * mapped.RowPitch,
					copyBytesPerLine
				);
			}
		}

		m_d3dContext->Unmap(m_stagingTexture.Get(), 0);
		m_hasNewFrame.store(true, std::memory_order_release);
	}
	catch (...)
	{
		WC_LOG(Warning, TEXT("Capture session work failed"));
	}

	return FWCWorkerThread::EWorkState::Working;
}

int WindowCaptureSession::Start(HWND hWnd)
{
	m_hwnd = hWnd;

	if (!::IsWindow(hWnd))
	{
		WC_LOG(Warning, TEXT("Invalid window handle"));
		return -1;
	}

	__wchar_t windowTitle[1024];
	if (GetWindowText(hWnd, windowTitle, 1024) == 0)
	{
		WC_LOG(Warning, TEXT("Failed to get window title"));
		return -3;
	}

	if (m_state == CaptureState::Running) return 1;

	m_state = CaptureState::Running;

	m_workerThread = new FWCWorkerThread(
		[this]()
		{
			return CaptureWork();
		},
		[this]() { 
			winrt::init_apartment(winrt::apartment_type::multi_threaded);
			WC_LOG(Log, TEXT("Worker thread COM initialized"));
			
			InitializeCaptureResources();
            InitializeWinRTCaptureResources();
		},
		[this]() { 
			winrt::uninit_apartment(); 
			WC_LOG(Log, TEXT("Worker thread COM uninitialized"));
		});
		
	m_workerThreadHandle = FRunnableThread::Create(m_workerThread, TEXT("WindowCaptureSessionWorker"));

	return 0;
}

void WindowCaptureSession::Stop()
{
	if (m_state == CaptureState::Stopped) { return; }

	m_state = CaptureState::Stopped;

	FScopeLock lock(&m_mutex_thread);
	
	try
	{
		m_frameArrivedRevoker.revoke();

		if (m_workerThread)
		{
			m_workerThread->Stop();
		}

		if (m_workerThreadHandle)
		{
			m_workerThreadHandle->Kill(true);
			m_workerThreadHandle->WaitForCompletion();
			delete m_workerThreadHandle;
			m_workerThreadHandle = nullptr;
		}

		if (m_workerThread)
		{
			delete m_workerThread;
			m_workerThread = nullptr;
		}

		if (m_session)
		{
			m_session.Close();
		}

		if (m_framePool)
		{
			m_framePool.Close();
		}		
	}
	catch (...)
	{
		WC_LOG(Warning, TEXT("Capture session stop failed"));
	}

	m_session = nullptr;
	m_framePool = nullptr;
	m_captureItem = nullptr;
	m_d3dDevice = nullptr;
	m_d3dContext = nullptr;
	m_stagingTexture = nullptr;	
}
#endif