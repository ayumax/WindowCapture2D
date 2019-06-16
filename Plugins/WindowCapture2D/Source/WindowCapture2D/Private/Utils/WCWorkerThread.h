// Copyright 2019 ayumax. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Runtime/Core/Public/HAL/Runnable.h"
#include "Runtime/Core/Public/HAL/RunnableThread.h"


class WINDOWCAPTURE2D_API FWCWorkerThread : public FRunnable
{
public:
	FWCWorkerThread(TFunction<bool()> InWork, float WaitSeconds = 0.001f);
	FWCWorkerThread(TFunction<bool()> InWork, TFunction<void()> InEnd, float WaitSeconds = 0.001f);
	~FWCWorkerThread();

	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	TFunction<bool()> Work;
	TFunction<void()> End;
	float Seconds;
	volatile bool ContinueRun;
};
