// Copyright 2019 ayumax. All Rights Reserved.
#ifndef PLATFORM_WINDOWS
#include "CaptureMachine.h"
#include "Engine/Texture2D.h"

UCaptureMachine::UCaptureMachine()
{
}

void UCaptureMachine::Start()
{
}

void UCaptureMachine::Dispose()
{
}

bool UCaptureMachine::TickCapture(float deltaTime)
{
	return true;
}


UTexture2D* UCaptureMachine::CreateTexture()
{
	return nullptr;	
}

bool UCaptureMachine::FindTargetWindow(HWND hWnd)
{
	return true;
}

void UCaptureMachine::GetWindowSize(HWND hWnd)
{
}

void UCaptureMachine::ReCreateTexture()
{
}

#endif
