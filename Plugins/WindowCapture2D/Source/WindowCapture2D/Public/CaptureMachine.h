// Copyright 2019 ayumax. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureMachineProperties.h"
#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include <WinUser.h>
#else
using HBITMAP = void*;
using HDC = void*;
using HWND = void*;
#endif
#include "CaptureMachine.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCaptureMachineChangeTexture, UTexture2D*, NewTexture);


UCLASS(BlueprintType, Blueprintable)
class WINDOWCAPTURE2D_API ACaptureMachine : public AActor
{
	GENERATED_BODY()
	
public:	
	ACaptureMachine();

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintPure, Category = WindowCapture2D)
	UTexture2D* CreateTexture();

	bool FindTargetWindow(HWND hWnd);
	void UpdateTexture();
	void GetWindowSize(HWND hWnd);
	void ReCreateTexture();
	bool DoCapture();

public:	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WindowCapture2D)
	FCaptureMachineProperties Properties;

	UPROPERTY(BlueprintReadOnly, Category = SceneCapture)
	class UTexture2D* TextureTarget;

	UPROPERTY(BlueprintAssignable, Category = SceneCapture)
	FCaptureMachineChangeTexture ChangeTexture;

private:
	char* m_BitmapBuffer = nullptr;

	HBITMAP m_hBmp = nullptr;
	HDC m_MemDC = nullptr;
	HBITMAP m_hOriginalBmp = nullptr;
	HDC m_OriginalMemDC = nullptr;
	HWND m_TargetWindow = nullptr;

	FIntVector2D m_WindowSize;
	FIntVector2D m_OriginalWindowSize;
	FIntVector2D m_WindowOffset;

	class FWCWorkerThread* CaptureWorkerThread = nullptr;
	class FRunnableThread* CaptureThread = nullptr;
};
