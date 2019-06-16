// Copyright 2019 ayumax. All Rights Reserved.
#include "WCWorkerThread.h"
#include "Runtime/Core/Public/HAL/PlatformProcess.h"

FWCWorkerThread::FWCWorkerThread(TFunction<bool()> InWork, float WaitSeconds)
	: Work(InWork)
	, End([]() {})
	, Seconds(WaitSeconds)
	, ContinueRun(true)
{

}

FWCWorkerThread::FWCWorkerThread(TFunction<bool()> InWork, TFunction<void()> InEnd, float WaitSeconds)
	: Work(InWork)
	, End(InEnd)
	, Seconds(WaitSeconds)
	, ContinueRun(true)
{

}

FWCWorkerThread::~FWCWorkerThread()
{

}

uint32 FWCWorkerThread::Run()
{
	while (ContinueRun)
	{
		FDateTime startTime = FDateTime::Now();

		if (!Work())
		{
			return 0;
		}


		if (ContinueRun)
		{
			float waitTime = FMath::Max(0.01f, Seconds - (float)(FDateTime::Now() - startTime).GetTotalSeconds());
			FPlatformProcess::Sleep(waitTime);
		}
	}

	return 0;
}

void FWCWorkerThread::Stop()
{
	ContinueRun = false;

}

void FWCWorkerThread::Exit()
{
	ContinueRun = false;

	End();
}
