// Copyright 2019 ayumax. All Rights Reserved.

#include "CaptureMachine.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"
#include "Async/Async.h"

#if PLATFORM_WINDOWS
#include <dwmapi.h>
#endif

ACaptureMachine::ACaptureMachine()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ACaptureMachine::BeginDestroy()
{
	Super::BeginDestroy();

#if PLATFORM_WINDOWS
	if (m_hBmp)
	{
		::DeleteObject(m_hBmp);
		m_BitmapBuffer = nullptr;
	}
#endif
}

void ACaptureMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

#if PLATFORM_WINDOWS
	if (!TextureTarget) return;

	if (CheckWindowSize)
	{
		FIntVector2D oldWindowSize = m_WindowSize;
		GetWindowSize(m_TargetWindow);
		if (m_WindowSize != oldWindowSize)
		{
			ReCreateTexture();
			ChangeTexture.Broadcast(TextureTarget);
		}

		if (!TextureTarget) return;
	}

	AsyncTask(ENamedThreads::BackgroundThreadPriority, [this]() {
		if (CutShadow)
		{
			::PrintWindow(m_TargetWindow, m_OriginalMemDC, 2);
			::BitBlt(m_MemDC, 0, 0, m_WindowSize.X, m_WindowSize.Y, m_OriginalMemDC, m_WindowOffset.X, m_WindowOffset.Y, SRCCOPY);
		}
		else
		{
			::PrintWindow(m_TargetWindow, m_MemDC, 2);
		}
		});
	

	UpdateTexture();
#endif
}

UTexture2D* ACaptureMachine::CreateTexture()
{
#if PLATFORM_WINDOWS
	m_TargetWindow = nullptr;

	::EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
		{
			ACaptureMachine* my = (ACaptureMachine*)lParam;
			__wchar_t windowTitle[1024];
			GetWindowText(hwnd, windowTitle, 1024);
			FString title(windowTitle);
			if (title.StartsWith(my->CaptureTargetTitle, ESearchCase::IgnoreCase))
			{
				my->m_TargetWindow = hwnd;
				return false;
			}

			return true;
		}, (LPARAM)this);

	if (!m_TargetWindow) return nullptr;

	GetWindowSize(m_TargetWindow);

	HDC foundDC = ::GetDC(m_TargetWindow);
	m_MemDC = ::CreateCompatibleDC(foundDC);
	if (CutShadow)
	{
		m_OriginalMemDC = ::CreateCompatibleDC(foundDC);
	}

	ReleaseDC(m_TargetWindow, foundDC);

	ReCreateTexture();

	return TextureTarget;
#endif
	return nullptr;
}

void ACaptureMachine::UpdateTexture()
{
#if PLATFORM_WINDOWS
	if (!TextureTarget) return;

	auto Region = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureTarget->GetSizeX(), TextureTarget->GetSizeY());
	TextureTarget->UpdateTextureRegions(0, 1, Region, 4 * TextureTarget->GetSizeX(), 4, (uint8*)m_BitmapBuffer);
#endif
}

void ACaptureMachine::GetWindowSize(HWND hWnd)
{
#if PLATFORM_WINDOWS
	if (!::IsWindow(hWnd))
	{
		m_OriginalWindowSize = FIntVector2D(0, 0);
		m_WindowSize = m_OriginalWindowSize;
		m_WindowOffset = FIntVector2D(0, 0);
		return;
	}

	RECT rect;
	::GetWindowRect(hWnd, &rect);

	if (CutShadow)
	{
		RECT dwmWindowRect;
		::DwmGetWindowAttribute(hWnd, DWMWA_EXTENDED_FRAME_BOUNDS, &dwmWindowRect, sizeof(RECT));

		m_OriginalWindowSize = FIntVector2D(rect.right - rect.left, rect.bottom - rect.top);
		m_WindowSize = FIntVector2D(dwmWindowRect.right - dwmWindowRect.left, dwmWindowRect.bottom - dwmWindowRect.top);
		m_WindowOffset = FIntVector2D(dwmWindowRect.left - rect.left, dwmWindowRect.top - rect.top);
	}
	else
	{
		m_OriginalWindowSize = FIntVector2D(rect.right - rect.left, rect.bottom - rect.top);
		m_WindowSize = m_OriginalWindowSize;
		m_WindowOffset = FIntVector2D(0, 0);
	}
#endif
}

void ACaptureMachine::ReCreateTexture()
{
#if PLATFORM_WINDOWS
	if (m_hBmp)
	{
		::DeleteObject(m_hBmp);
		m_BitmapBuffer = nullptr;
	}

	if (m_WindowSize.X == 0 || m_WindowSize.Y == 0)
	{
		TextureTarget = nullptr;
		return;
	}

	m_BitmapBuffer = new char[m_WindowSize.X * m_WindowSize.Y * 4];

	TextureTarget = UTexture2D::CreateTransient(m_WindowSize.X, m_WindowSize.Y, PF_B8G8R8A8);
	TextureTarget->UpdateResource();

	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = m_WindowSize.X;
	bmpInfo.bmiHeader.biHeight = m_WindowSize.Y;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	m_hBmp = ::CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, (void**)&m_BitmapBuffer, NULL, 0);

	::SelectObject(m_MemDC, m_hBmp);

	if (CutShadow)
	{
		m_hOriginalBmp = ::CreateCompatibleBitmap(m_MemDC, m_OriginalWindowSize.X, m_OriginalWindowSize.Y);
		::SelectObject(m_OriginalMemDC, m_hOriginalBmp);
	}
#endif
}