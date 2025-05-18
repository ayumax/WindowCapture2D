// Copyright 2019 ayumax. All Rights Reserved.
#include "WCWorkerThread.h"
#include "Runtime/Core/Public/HAL/PlatformProcess.h"
#include "HAL/Event.h"
#include "WindowCapture2DMacros.h"

FWCWorkerThread::FWCWorkerThread(
	const TFunction<EWorkState()>& InWork,
	const TFunction<void()>& InInitialize,
	const TFunction<void()>& InFinalize)
	: ContinueRun(true)
	, Work(InWork)
	, Initialize(InInitialize)
	, Finalize(InFinalize)
{
}

FWCWorkerThread::~FWCWorkerThread()
{
}

uint32 FWCWorkerThread::Run()
{
	try
    {
		Initialize();
		
		while (ContinueRun)
		{			
			if (!ContinueRun)
			{
				break;
			}

			switch (auto state = Work())
			{
			case EWorkState::Working:
				break;
			case EWorkState::Wait:
				FPlatformProcess::Sleep(0.001f);
				break;
			case EWorkState::Stop:
				ContinueRun = false;
				break;
			}
		}

		Finalize();
    }
	catch (const std::exception& e)
	{
		WC_LOG(Error, TEXT("Worker thread error: %hs"), e.what());
		Finalize();
		return 1;
	}
	catch (...)
	{
		WC_LOG(Error, TEXT("Error in worker thread"));
		Finalize();
		return 2;
	}	
	
	return 0;
}

void FWCWorkerThread::Stop()
{
	ContinueRun = false;
	
	FRunnable::Stop();
}

void FWCWorkerThread::Exit()
{
	if (ContinueRun)
	{
		ContinueRun = false;
	}

	FRunnable::Exit();
}
