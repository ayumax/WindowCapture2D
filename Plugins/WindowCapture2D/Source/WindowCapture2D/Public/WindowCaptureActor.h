// Copyright 2019 ayumax. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CaptureMachineProperties.h"
#include "CaptureMachine.h"
#include "WindowCaptureActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWindowCaptureActorChangeTexture, UTexture2D*, NewTexture);


UCLASS(BlueprintType, Blueprintable)
class WINDOWCAPTURE2D_API AWindowCaptureActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AWindowCaptureActor();

protected:
	UFUNCTION(BlueprintCallable, Category = WindowCapture2D)
	UTexture2D* Start();

	UFUNCTION()
	void OnChangeTexture(UTexture2D* NewTexture);

public:	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WindowCapture2D)
	FCaptureMachineProperties Properties;

	UPROPERTY(BlueprintReadOnly, Category = SceneCapture)
	class UTexture2D* TextureTarget;

	UPROPERTY(BlueprintAssignable, Category = SceneCapture)
	FWindowCaptureActorChangeTexture ChangeTexture;

protected:
	UPROPERTY(Transient)
	UCaptureMachine* CaptureMachine = nullptr;
};
