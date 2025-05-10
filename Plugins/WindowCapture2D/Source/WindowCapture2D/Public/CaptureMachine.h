// Copyright 2019 ayumax. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CaptureMachineProperties.h"
#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include <WinUser.h>
#else
using HWND = void*;
#endif
#include "HAL/CriticalSection.h"
#include "Containers/Array.h"
#include "HAL/PlatformAtomics.h"
#include "CaptureMachine.generated.h"

class WindowCaptureSession;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCaptureMachineChangeTexture, UTexture2D*, NewTexture);


UCLASS(BlueprintType, Blueprintable)
class WINDOWCAPTURE2D_API UCaptureMachine : public UObject
{
	GENERATED_BODY()
	
public:	
	UCaptureMachine();

	virtual void Start();

	virtual void Dispose();

	bool TickCapture(float deltaTime);

	UFUNCTION(BlueprintPure, Category = WindowCapture2D)
	UTexture2D* CreateTexture();

protected:
	bool FindTargetWindow(HWND hWnd);
	void GetWindowSize(HWND hWnd);
	void ReCreateTexture();
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WindowCapture2D)
	FCaptureMachineProperties Properties;

	UPROPERTY(BlueprintReadOnly, Category = SceneCapture)
	class UTexture2D* TextureTarget;

	UPROPERTY(BlueprintAssignable, Category = SceneCapture)
	FCaptureMachineChangeTexture ChangeTexture;

private:
	HWND m_TargetWindow = nullptr;

	FIntVector2D m_WindowSize;	

	WindowCaptureSession* m_Session = nullptr;
	
	FTSTicker::FDelegateHandle _TickerHandle;
	TAtomic<uint8> _IsDisposed = 0;

	FTSTicker::FDelegateHandle _TickHandle;
};
