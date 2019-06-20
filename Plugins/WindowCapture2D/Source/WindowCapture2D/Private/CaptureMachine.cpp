// Copyright 2019 ayumax. All Rights Reserved.

#include "CaptureMachine.h"
#include "Engine/Texture2D.h"
#include "Async/Async.h"
#include "Internationalization/Regex.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"
#include "../Private/Utils/WCWorkerThread.h"

#if PLATFORM_WINDOWS
#include <dwmapi.h>
#endif

UCaptureMachine::UCaptureMachine()
{
}

void UCaptureMachine::Start()
{
#if PLATFORM_WINDOWS
	CaptureWorkerThread = new FWCWorkerThread([this] { return DoCapture(); }, 1.0f / (float)Properties.FrameRate);
	CaptureThread = FRunnableThread::Create(CaptureWorkerThread, TEXT("UCaptureMachine CaptureThread"));
#endif
}

void UCaptureMachine::Close()
{
#if PLATFORM_WINDOWS

	if (TextureTarget)
	{
		TextureTarget->ReleaseResource();
	}

	if (CaptureThread)
	{
		CaptureThread->Kill(true);
		CaptureThread->WaitForCompletion();

		delete CaptureThread;
		CaptureThread = nullptr;
	}

	if (CaptureWorkerThread)
	{
		delete CaptureWorkerThread;
		CaptureWorkerThread = nullptr;
	}

	if (m_hBmp)
	{
		::DeleteObject(m_hBmp);
		m_BitmapBuffer = nullptr;
	}

	if (m_MemDC)
	{
		::DeleteDC(m_MemDC);
		m_MemDC = nullptr;
	}

	if (m_hOriginalBmp)
	{
		::DeleteObject(m_hOriginalBmp);
		m_hOriginalBmp = nullptr;
	}

	if (m_OriginalMemDC)
	{
		::DeleteDC(m_OriginalMemDC);
		m_OriginalMemDC = nullptr;
	}

#endif

}


bool UCaptureMachine::DoCapture()
{
#if PLATFORM_WINDOWS
	if (!m_TargetWindow) return true;
	if (!TextureTarget) return true;

	if (Properties.CheckWindowSize)
	{
		FIntVector2D oldWindowSize = m_WindowSize;
		GetWindowSize(m_TargetWindow);
		if (m_WindowSize != oldWindowSize)
		{
			ReCreateTexture();
			ChangeTexture.Broadcast(TextureTarget);
		}

		if (!TextureTarget) return true;
	}


	if (Properties.CutShadow)
	{
		::PrintWindow(m_TargetWindow, m_OriginalMemDC, 2);
		::BitBlt(m_MemDC, 0, 0, m_WindowSize.X, m_WindowSize.Y, m_OriginalMemDC, m_WindowOffset.X, m_WindowOffset.Y, SRCCOPY);
	}
	else
	{
		::PrintWindow(m_TargetWindow, m_MemDC, 2);
	}

	UpdateTexture();
#endif

	return true;
}

UTexture2D* UCaptureMachine::CreateTexture()
{
#if PLATFORM_WINDOWS
	m_TargetWindow = nullptr;

	::EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
		{
			UCaptureMachine* my = (UCaptureMachine*)lParam;
			return my->FindTargetWindow(hwnd);
		}, (LPARAM)this);

	if (!m_TargetWindow) return nullptr;

	GetWindowSize(m_TargetWindow);

	HDC foundDC = ::GetDC(m_TargetWindow);
	m_MemDC = ::CreateCompatibleDC(foundDC);
	if (Properties.CutShadow)
	{
		m_OriginalMemDC = ::CreateCompatibleDC(foundDC);
	}

	ReleaseDC(m_TargetWindow, foundDC);

	ReCreateTexture();

	return TextureTarget;
#endif
	return nullptr;
}

bool UCaptureMachine::FindTargetWindow(HWND hWnd)
{
#if PLATFORM_WINDOWS
	__wchar_t windowTitle[1024];
	GetWindowText(hWnd, windowTitle, 1024);
	FString title(windowTitle);

	if (title.IsEmpty()) return true;

	bool isMatch = false;

	switch (Properties.TitleMatchingWindowSearch)
	{
	case ETitleMatchingWindowSearch::PerfectMatch:
		isMatch = title.Equals(Properties.CaptureTargetTitle, ESearchCase::IgnoreCase);
		break;

	case ETitleMatchingWindowSearch::ForwardMatch:
		isMatch = title.StartsWith(Properties.CaptureTargetTitle, ESearchCase::IgnoreCase);
		break;

	case ETitleMatchingWindowSearch::PartialMatch:
		isMatch = title.Contains(Properties.CaptureTargetTitle, ESearchCase::IgnoreCase);
		break;

	case ETitleMatchingWindowSearch::BackwardMatch:
		isMatch = title.EndsWith(Properties.CaptureTargetTitle, ESearchCase::IgnoreCase);
		break;

	case ETitleMatchingWindowSearch::RegularExpression:
		{
			const FRegexPattern pattern = FRegexPattern(Properties.CaptureTargetTitle);
			FRegexMatcher matcher(pattern, title);

			isMatch = matcher.FindNext();
		}
		break;
	}

	if (isMatch)
	{
		m_TargetWindow = hWnd;
		return false;
	}
#endif

	return true;
}

void UCaptureMachine::UpdateTexture()
{
#if PLATFORM_WINDOWS
	if (!TextureTarget) return;

	auto Region = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureTarget->GetSizeX(), TextureTarget->GetSizeY());
	TextureTarget->UpdateTextureRegions(0, 1, Region, 4 * TextureTarget->GetSizeX(), 4, (uint8*)m_BitmapBuffer);
#endif
}

void UCaptureMachine::GetWindowSize(HWND hWnd)
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

	if (Properties.CutShadow)
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

void UCaptureMachine::ReCreateTexture()
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

	if (Properties.CutShadow)
	{
		m_hOriginalBmp = ::CreateCompatibleBitmap(m_MemDC, m_OriginalWindowSize.X, m_OriginalWindowSize.Y);
		::SelectObject(m_OriginalMemDC, m_hOriginalBmp);
	}
#endif
}