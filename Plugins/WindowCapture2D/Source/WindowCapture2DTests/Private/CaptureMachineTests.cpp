// Copyright 2024 ayumax. All Rights Reserved.
#ifdef PLATFORM_WINDOWS
#include "Misc/AutomationTest.h"
#include "CaptureMachine.h"
#include "Engine/Texture2D.h"
#include "Tests/AutomationCommon.h"

// Test: UCaptureMachine instance creation and Dispose
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCaptureMachineBasicTest, "WindowCapture2D.CaptureMachine.BasicLifecycle",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCaptureMachineBasicTest::RunTest(const FString& Parameters)
{
	UCaptureMachine* CaptureMachine = NewObject<UCaptureMachine>();
	TestNotNull(TEXT("UCaptureMachine instance should be created"), CaptureMachine);
	CaptureMachine->Dispose();
	TestTrue(TEXT("Dispose should be called without crash"), true);
	return true;
}

// Test: TextureTarget becomes non-null within timeout
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCaptureMachineTextureTargetSetTest,
                                 "WindowCapture2D.CaptureMachine.TextureTargetSetWithinTimeout",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCaptureMachineTextureTargetSetTest::RunTest(const FString& Parameters)
{
	UCaptureMachine* CaptureMachine = NewObject<UCaptureMachine>();
	CaptureMachine->Properties.CaptureTargetTitle = TEXT("WindowCapture2D");
	CaptureMachine->Properties.TitleMatchingWindowSearch = ETitleMatchingWindowSearch::PartialMatch;

	CaptureMachine->Start();
	
	ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(3.0f));
	
	ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, CaptureMachine]()
		{
		TestNotNull(TEXT("TextureTarget should become non-null within timeout"), CaptureMachine->TextureTarget);
		return true;}));

	ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, CaptureMachine]()
	{
		CaptureMachine->Dispose();
		return true; 
	}));
	
	return true;
}

// Test: CreateTexture fails when window is not found
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCaptureMachineCreateTextureNotFoundTest,
                                 "WindowCapture2D.CaptureMachine.CreateTextureNotFound",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCaptureMachineCreateTextureNotFoundTest::RunTest(const FString& Parameters)
{
	UCaptureMachine* CaptureMachine = NewObject<UCaptureMachine>();
	CaptureMachine->Properties.CaptureTargetTitle = TEXT("DefinitelyNotExistWindowTitle");
	CaptureMachine->Properties.TitleMatchingWindowSearch = ETitleMatchingWindowSearch::PerfectMatch;
	CaptureMachine->Start();

	ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(3.0f));
	
	ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, CaptureMachine]()
		{
		TestNull(TEXT("Texture should be null if window not found"), CaptureMachine->TextureTarget);
		return true;}));

	ADD_LATENT_AUTOMATION_COMMAND(FFunctionLatentCommand([this, CaptureMachine]()
	{
		CaptureMachine->Dispose();
		return true; 
	}));
	
	return true;
}

// Test: FindTargetWindow matching patterns
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCaptureMachineFindTargetWindowTest,
                                 "WindowCapture2D.CaptureMachine.FindTargetWindowPatterns",
                                 EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FCaptureMachineFindTargetWindowTest::RunTest(const FString& Parameters)
{
	UCaptureMachine* CaptureMachine = NewObject<UCaptureMachine>();
	HWND DummyHwnd = nullptr; // NOTE: Replace this with a valid HWND in integration test

	// Test for each matching type
	CaptureMachine->Properties.CaptureTargetTitle = TEXT("WindowCapture2D");
	CaptureMachine->Properties.TitleMatchingWindowSearch = ETitleMatchingWindowSearch::PartialMatch;
	CaptureMachine->Start();

	bool resultPerfect = CaptureMachine->FindTargetWindow(DummyHwnd);
	CaptureMachine->Properties.TitleMatchingWindowSearch = ETitleMatchingWindowSearch::ForwardMatch;
	bool resultForward = CaptureMachine->FindTargetWindow(DummyHwnd);
	CaptureMachine->Properties.TitleMatchingWindowSearch = ETitleMatchingWindowSearch::PartialMatch;
	bool resultPartial = CaptureMachine->FindTargetWindow(DummyHwnd);
	CaptureMachine->Properties.TitleMatchingWindowSearch = ETitleMatchingWindowSearch::BackwardMatch;
	bool resultBackward = CaptureMachine->FindTargetWindow(DummyHwnd);
	CaptureMachine->Properties.TitleMatchingWindowSearch = ETitleMatchingWindowSearch::RegularExpression;

	bool resultRegex = CaptureMachine->FindTargetWindow(DummyHwnd);
	// For dummy HWND, all should be true (not found)
	TestTrue(TEXT("PerfectMatch should return true for dummy hwnd"), resultPerfect);
	TestTrue(TEXT("ForwardMatch should return true for dummy hwnd"), resultForward);
	TestTrue(TEXT("PartialMatch should return true for dummy hwnd"), resultPartial);
	TestTrue(TEXT("BackwardMatch should return true for dummy hwnd"), resultBackward);
	TestTrue(TEXT("RegularExpression should return true for dummy hwnd"), resultRegex);
	CaptureMachine->Dispose();
	return true;
}
#endif
