// Copyright 2019 ayumax. All Rights Reserved.
#include "WCWorkerThread.h"
#include "Runtime/Core/Public/HAL/PlatformProcess.h"
#include "HAL/Event.h"

FWCWorkerThread::FWCWorkerThread(const TFunction<bool()>& InWork)
	: ContinueRun(true)
	, Work(InWork)
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
			return 0;
		}
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
