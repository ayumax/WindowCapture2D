// Copyright 2025 ayumax. All Rights Reserved.
#ifdef PLATFORM_WINDOWS
#include "Misc/AutomationTest.h"
#include "WindowCaptureSession.h"
#include <windows.h>
#include "WindowCapture2DTestHelper.h"

#define DUMMY_WINDOW_NAME L"DummyWindowForCapture"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindowCaptureSession_StartStopTest, "WindowCapture2D.WindowCaptureSession.StartStop", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWindowCaptureSession_StartStopTest::RunTest(const FString& Parameters)
{
    HWND dummyWnd = WindowCapture2DTestHelper::CreateWindowForCapture(DUMMY_WINDOW_NAME);
    
    HWND hwnd = WindowCapture2DTestHelper::FindWindowByPrefix(DUMMY_WINDOW_NAME);
    TestNotNull(TEXT("Target window should exist"), hwnd);
    WindowCaptureSession session;
    int result = session.Start(hwnd);
    TestEqual(TEXT("Start returns 0 (success)"), result, 0);
    WindowCapture2DTestHelper::WaitForNewFrame(session);
    session.Stop();
    
    DestroyWindow(dummyWnd);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindowCaptureSession_HasNewFrameTest, "WindowCapture2D.WindowCaptureSession.HasNewFrame", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWindowCaptureSession_HasNewFrameTest::RunTest(const FString& Parameters)
{
    HWND dummyWnd = WindowCapture2DTestHelper::CreateWindowForCapture(DUMMY_WINDOW_NAME);

    HWND hwnd = WindowCapture2DTestHelper::FindWindowByPrefix(DUMMY_WINDOW_NAME);
    TestNotNull(TEXT("Target window should exist"), hwnd);
    WindowCaptureSession session;
    session.Start(hwnd);
    bool hasFrame = WindowCapture2DTestHelper::WaitForNewFrame(session);
    TestTrue(TEXT("HasNewFrame should be true after start and wait"), hasFrame);
    session.Stop();
    hasFrame = session.HasNewFrame();
    TestFalse(TEXT("HasNewFrame should be false after stop"), hasFrame);
    
    DestroyWindow(dummyWnd);
    
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindowCaptureSession_GetFrameInfoTest, "WindowCapture2D.WindowCaptureSession.GetFrameInfo", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWindowCaptureSession_GetFrameInfoTest::RunTest(const FString& Parameters)
{
    HWND dummyWnd = WindowCapture2DTestHelper::CreateWindowForCapture(DUMMY_WINDOW_NAME);

    HWND hwnd = WindowCapture2DTestHelper::FindWindowByPrefix(DUMMY_WINDOW_NAME);
    TestNotNull(TEXT("Target window should exist"), hwnd);
    WindowCaptureSession session;
    session.Start(hwnd);
    WindowCapture2DTestHelper::WaitForNewFrame(session);
    WCFrameDesc desc;
    bool ok = session.GetFrameInfo(&desc);
    TestTrue(TEXT("GetFrameInfo should return true"), ok);
    TestTrue(TEXT("Width should be > 0"), desc.width > 0);
    TestTrue(TEXT("Height should be > 0"), desc.height > 0);
    TestEqual(TEXT("BytesPerPixel should be 4"), desc.bytesPerPixel, 4);
    session.Stop();

    DestroyWindow(dummyWnd);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FWindowCaptureSession_StartWithInvalidHwndTest, "WindowCapture2D.WindowCaptureSession.StartWithInvalidHwnd", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FWindowCaptureSession_StartWithInvalidHwndTest::RunTest(const FString& Parameters)
{
    HWND hwnd = (HWND)0x12345678; // Invalid handle
    WindowCaptureSession session;
    int result = session.Start(hwnd);
    TestEqual(TEXT("Start should fail with invalid hwnd"), result, -1);
    WindowCapture2DTestHelper::WaitForNewFrame(session);
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
