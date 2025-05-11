// Copyright 2025 ayumax. All Rights Reserved.
#ifdef PLATFORM_WINDOWS
#include "Misc/AutomationTest.h"
#include "WindowCaptureSession.h"
#include <windows.h>

// Helper function to find a window by title prefix
HWND FindWindowByPrefix(const wchar_t* prefix)
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
bool WaitForNewFrame(WindowCaptureSession& session, int timeoutMs = 2000)
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindowCaptureSession_StartStopTest, "WindowCapture2D.WindowCaptureSession.StartStop", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWindowCaptureSession_StartStopTest::RunTest(const FString& Parameters)
{
    HWND hwnd = FindWindowByPrefix(L"WindowCapture2D");
    TestNotNull(TEXT("Target window should exist"), hwnd);
    WindowCaptureSession session;
    int result = session.Start(hwnd);
    TestEqual(TEXT("Start returns 0 (success)"), result, 0);
    session.Stop();
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindowCaptureSession_HasNewFrameTest, "WindowCapture2D.WindowCaptureSession.HasNewFrame", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWindowCaptureSession_HasNewFrameTest::RunTest(const FString& Parameters)
{
    HWND hwnd = FindWindowByPrefix(L"WindowCapture2D");
    TestNotNull(TEXT("Target window should exist"), hwnd);
    WindowCaptureSession session;
    session.Start(hwnd);
    bool hasFrame = WaitForNewFrame(session);
    TestTrue(TEXT("HasNewFrame should be true after start and wait"), hasFrame);
    session.Stop();
    hasFrame = session.HasNewFrame();
    TestFalse(TEXT("HasNewFrame should be false after stop"), hasFrame);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindowCaptureSession_GetFrameInfoTest, "WindowCapture2D.WindowCaptureSession.GetFrameInfo", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWindowCaptureSession_GetFrameInfoTest::RunTest(const FString& Parameters)
{
    HWND hwnd = FindWindowByPrefix(L"WindowCapture2D");
    TestNotNull(TEXT("Target window should exist"), hwnd);
    WindowCaptureSession session;
    session.Start(hwnd);
    Sleep(100);
    WCFrameDesc desc;
    bool ok = session.GetFrameInfo(&desc);
    TestTrue(TEXT("GetFrameInfo should return true"), ok);
    TestTrue(TEXT("Width should be > 0"), desc.width > 0);
    TestTrue(TEXT("Height should be > 0"), desc.height > 0);
    TestEqual(TEXT("BytesPerPixel should be 4"), desc.bytesPerPixel, 4);
    session.Stop();
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindowCaptureSession_StartWithInvalidHwndTest, "WindowCapture2D.WindowCaptureSession.StartWithInvalidHwnd", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWindowCaptureSession_StartWithInvalidHwndTest::RunTest(const FString& Parameters)
{
    HWND hwnd = (HWND)0x12345678; // Invalid handle
    WindowCaptureSession session;
    int result = session.Start(hwnd);
    TestNotEqual(TEXT("Start should fail with invalid hwnd"), result, 0);
    session.Stop();
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindowCaptureSession_UseBufferThreadSafetyTest, "WindowCapture2D.WindowCaptureSession.UseBufferThreadSafety", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWindowCaptureSession_UseBufferThreadSafetyTest::RunTest(const FString& Parameters)
{
    WindowCaptureSession session;
    bool flag = false;
    session.UseBuffer([&]() { flag = true; });
    TestTrue(TEXT("UseBuffer should execute action in lock"), flag);
    return true;
}
#endif
