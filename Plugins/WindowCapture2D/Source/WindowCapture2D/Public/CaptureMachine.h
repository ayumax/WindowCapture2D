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
#include "Containers/Ticker.h"
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

	UFUNCTION(BlueprintCallable, Category = WindowCapture2D)
	virtual void Start();

	UFUNCTION(BlueprintCallable, Category = WindowCapture2D)
	virtual void Dispose();

	bool TickCapture(float deltaTime);

	bool FindTargetWindow(HWND hWnd);
	
protected:
	void GetWindowSize();
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
