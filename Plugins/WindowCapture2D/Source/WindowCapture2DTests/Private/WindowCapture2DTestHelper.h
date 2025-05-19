#pragma once
#ifdef PLATFORM_WINDOWS
#include <windows.h>
#include "WindowCaptureSession.h"

class WindowCapture2DTestHelper
{
public:
	// Helper function to find a window by title prefix
	static HWND FindWindowByPrefix(const wchar_t* prefix)
	{
		HWND hwnd = nullptr;
		HWND current = nullptr;
		wchar_t title[256];
		while ((current = FindWindowExW(nullptr, current, nullptr, nullptr)) != nullptr)
		{
			GetWindowTextW(current, title, 256);
			if (wcsncmp(title, prefix, wcslen(prefix)) == 0)
			{
				hwnd = current;
				break;
			}
		}
		return hwnd;
	}

	// Wait for HasNewFrame to become true, processing messages
	static bool WaitForNewFrame(WindowCaptureSession& session, int timeoutMs = 2000)
	{
		const int interval = 10;
		int elapsed = 0;
		while (elapsed < timeoutMs)
		{
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (session.HasNewFrame())
				return true;
			Sleep(interval);
			elapsed += interval;
		}
		return false;
	}

	// Create a window with the specified title
	static HWND CreateWindowForCapture(const wchar_t* title)
	{
		WNDCLASSEX wc = { 0 };
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.lpfnWndProc = DefWindowProc;
		wc.hInstance = GetModuleHandle(nullptr);
		wc.lpszClassName = L"CaptureTestWindow";
		RegisterClassEx(&wc);

		HWND hwnd = CreateWindowEx(0, wc.lpszClassName, title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300, 200, nullptr, nullptr, wc.hInstance, nullptr);
		if (!hwnd)
			return nullptr;

		ShowWindow(hwnd, SW_SHOW);

		return hwnd;
	}
};
#endif