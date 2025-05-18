// Copyright 2025 ayumax. All Rights Reserved.
#pragma once
#ifdef PLATFORM_WINDOWS
#pragma warning(disable : 5205 4265 4268 4946)
#include "CoreMinimal.h"
#include "HAL/CriticalSection.h"
#include "Windows/WindowsHWrapper.h"
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/AllowWindowsPlatformAtomics.h"
#include "Windows/PreWindowsApi.h"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <wrl/client.h>
#include <winrt/Windows.Graphics.Capture.h>
#include <winrt/Windows.Graphics.DirectX.Direct3D11.h>
#include <atomic>
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformAtomics.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "WCWorkerThread.h"

class WindowCaptureSession;

typedef struct WCFrameDesc {
	int width;
	int height;
	int stride;
	int bytesPerPixel;
} WCFrameDesc;


class WINDOWCAPTURE2D_API WindowCaptureSession
{
public:
	WindowCaptureSession();
	~WindowCaptureSession();

	HWND GetHwnd() const { return m_hwnd; }

	int Start(HWND hWnd);
	void Stop();
	
	bool HasNewFrame() const;
	bool GetFrameInfo(WCFrameDesc* OutDesc) const;
	void UseBuffer(const TFunction<void()>& Action);

	TArray<uint8> m_buffer;
private:

	FWCWorkerThread::EWorkState CaptureWork();
	
	HWND m_hwnd;
	FCriticalSection m_mutex_buffer;
	FCriticalSection m_mutex_thread;
	
	std::atomic<bool> m_hasNewFrame{false};
	
	enum class CaptureState { Stopped, Running };
	CaptureState m_state = CaptureState::Stopped;

	// --- WinRT/Direct3D11 resources ---
	Microsoft::WRL::ComPtr<ID3D11Device> m_d3dDevice;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_d3dContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain; 
	// Staging texture for CPU readback
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_stagingTexture;
	D3D11_TEXTURE2D_DESC m_stagingDesc = {};
	
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem m_captureItem{nullptr};
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_framePool{nullptr};
	winrt::Windows::Graphics::Capture::GraphicsCaptureSession m_session{nullptr};
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool::FrameArrived_revoker m_frameArrivedRevoker;


	void InitializeCaptureResources();
	void InitializeWinRTCaptureResources();

	class FWCWorkerThread* m_workerThread = nullptr;
	class FRunnableThread* m_workerThreadHandle = nullptr;
};
#endif
