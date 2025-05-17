// Copyright 2019 ayumax. All Rights Reserved.
#include "WCWorkerThread.h"
#include "Runtime/Core/Public/HAL/PlatformProcess.h"
#include "HAL/Event.h"
#include "WindowCapture2DMacros.h"

FWCWorkerThread::FWCWorkerThread(
	const TFunction<bool()>& InWork,
	const TFunction<void()>& InInitialize,
	const TFunction<void()>& InFinalize)
	: ContinueRun(true)
	, Work(InWork)
	, Initialize(InInitialize)
	, Finalize(InFinalize)
	, _waitEvent(nullptr)
{
	_waitEvent = FPlatformProcess::GetSynchEventFromPool(false);
}

FWCWorkerThread::~FWCWorkerThread()
{
	if (_waitEvent)
	{
		FPlatformProcess::ReturnSynchEventToPool(_waitEvent);
		_waitEvent = nullptr;
	}
}

uint32 FWCWorkerThread::Run()
{
	try
    {
		Initialize();

		while (ContinueRun)
		{
			if (_waitEvent)
			{
				_waitEvent->Wait(100);
			}
			
			if (!ContinueRun)
			{
				break;
			}
			
			if (!Work())
			{
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

	if (_waitEvent)
	{
		_waitEvent->Trigger();
	}

	FRunnable::Stop();
}

void FWCWorkerThread::Exit()
{
	if (ContinueRun)
	{
		ContinueRun = false;
    
    	if (_waitEvent)
    	{
    		_waitEvent->Trigger();
    	}
	}

	FRunnable::Exit();
}

void FWCWorkerThread::WakeUp()
{	
	if (_waitEvent)
	{
		_waitEvent->Trigger();
	}
}
