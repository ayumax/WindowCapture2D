// Copyright 2019 ayumax. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CaptureMachineProperties.generated.h"

struct FIntVector2D
{	
	int32 X;
	int32 Y;
	
	FIntVector2D()
		: X(0), Y(0)
	{

	}
	FIntVector2D(int32 _X, int32 _Y)
		: X(_X), Y(_Y)
	{
	}

	bool operator != (const FIntVector2D& obj)
	{
		return X != obj.X || Y != obj.Y;
	}
};

UENUM(BlueprintType)
enum class ETitleMatchingWindowSearch : uint8
{
	PerfectMatch,
	ForwardMatch,
	PartialMatch,
	BackwardMatch,
	RegularExpression
};



USTRUCT(BlueprintType)
struct WINDOWCAPTURE2D_API FCaptureMachineProperties 
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WindowCapture2D)
	FString CaptureTargetTitle = TEXT("WindowCapture2D");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WindowCapture2D)
	ETitleMatchingWindowSearch TitleMatchingWindowSearch = ETitleMatchingWindowSearch::ForwardMatch;

	// Frame rate limit: 1 to 120
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WindowCapture2D, meta = (ClampMin = 1, ClampMax = 120, UIMin = 1, UIMax = 120))
	int32 FrameRate = 30;
};
