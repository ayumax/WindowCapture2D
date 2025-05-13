// Copyright 2019 ayumax. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/Event.h"
#include "HAL/ThreadSafeBool.h"

class WINDOWCAPTURE2D_API FWCWorkerThread : public FRunnable
{
public:
	FWCWorkerThread(const TFunction<bool()>& InWork);
	~FWCWorkerThread();

	virtual uint32 Run() override final;
	virtual void Stop() override final;
	virtual void Exit() override;
	void WakeUp();

	FThreadSafeBool ContinueRun{true};

private:
	TFunction<bool()> Work;
	
	FEvent* _waitEvent;
};
