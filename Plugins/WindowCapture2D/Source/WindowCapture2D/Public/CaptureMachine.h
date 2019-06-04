// Copyright 2019 ayumax. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#include <WinUser.h>
#endif
#include "CaptureMachine.generated.h"

struct FIntVector2D
{	
	int32 X;
	int32 Y;

	FIntVector2D(int32 _X, int32 _Y)
	{
		X = _X;
		Y = _Y;
	}
};

UCLASS(BlueprintType, Blueprintable)
class WINDOWCAPTURE2D_API ACaptureMachine : public AActor
{
	GENERATED_BODY()
	
public:	
	ACaptureMachine();

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	UFUNCTION(BlueprintPure, Category = WindowCapture2D)
	UTexture2D* CreateTexture();

	void UpdateTexture();
	FIntVector2D GetWindowSize(HWND hWnd);
	void ReCreateTexture(FIntVector2D Size);

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WindowCapture2D)
	FString CaptureTargetTitle;

	UPROPERTY(BlueprintReadOnly, Category = SceneCapture)
	class UTexture2D* TextureTarget;

private:
	char* m_BitmapBuffer = nullptr;

	HBITMAP m_hBmp = nullptr;
	HDC m_MemDC = nullptr;
	HWND m_TargetWindow = nullptr;
};
