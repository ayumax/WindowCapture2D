// Copyright 2019 ayumax. All Rights Reserved.

#include "WindowCaptureWidget.h"
#include "Engine/Texture2D.h"


UWindowCaptureWidget::UWindowCaptureWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UWindowCaptureWidget::BeginDestroy()
{
	if (CaptureMachine)
	{
		CaptureMachine->Close();
		CaptureMachine = nullptr;
	}

	Super::BeginDestroy();
}

UTexture2D* UWindowCaptureWidget::Start()
{
	CaptureMachine = NewObject<UCaptureMachine>(this);

	CaptureMachine->Properties = Properties;

	CaptureMachine->ChangeTexture.AddDynamic(this, &UWindowCaptureWidget::OnChangeTexture);
	CaptureMachine->Start();

	return CaptureMachine->CreateTexture();
}

void UWindowCaptureWidget::OnChangeTexture(UTexture2D* _NewTexture)
{
	ChangeTexture.Broadcast(_NewTexture);
}