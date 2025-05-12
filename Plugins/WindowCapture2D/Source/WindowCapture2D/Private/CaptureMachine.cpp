// Copyright 2019 ayumax. All Rights Reserved.
#ifdef PLATFORM_WINDOWS
#include "CaptureMachine.h"
#include "Engine/Texture2D.h"
#include "Async/Async.h"
#include "Internationalization/Regex.h"
#include "WindowCaptureSession.h"
#include "WindowCapture2DMacros.h"

UCaptureMachine::UCaptureMachine()
{
}

UTexture2D* UCaptureMachine::Start()
{
	_TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UCaptureMachine::TickCapture), 1.0f / static_cast<float>(Properties.FrameRate));
	
	m_Session = new WindowCaptureSession();

	m_TargetWindow = nullptr;

	if (Properties.CaptureTargetTitle.IsEmpty())
	{
		WC_LOG(Log, "CaptureTargetTitle is empty : %s", *Properties.CaptureTargetTitle);
		return nullptr;
	}

	::EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
		{
			UCaptureMachine* me = reinterpret_cast<UCaptureMachine*>(lParam);
			return me->FindTargetWindow(hwnd);
		}, reinterpret_cast<LPARAM>(this));

	if (!m_TargetWindow || !::IsWindow(m_TargetWindow))
	{
		WC_LOG(Log, "Target window not found : %s", *Properties.CaptureTargetTitle);
		return nullptr;
	}

	m_Session->Start(m_TargetWindow);

	GetWindowSize();
	
	ReCreateTexture();
	
	return TextureTarget;
}

void UCaptureMachine::Dispose()
{
	if (_IsDisposed)
	{
		return;
	}
	
	WC_LOG(Log, "Dispose called", *Properties.CaptureTargetTitle);
	
	if (_TickHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(_TickHandle);
		_TickHandle.Reset();
	}
	
	if (m_Session)
	{
		m_Session->Stop();
		delete m_Session;
		m_Session = nullptr;
	}
	
	if (TextureTarget)
	{
		TextureTarget->ReleaseResource();
		TextureTarget = nullptr;
	}

	_IsDisposed = true;
}

bool UCaptureMachine::TickCapture(float deltaTime)
{
	if (_IsDisposed)
	{
		return false;
	}

	if (!m_Session->HasNewFrame()) {
		return true;
	}

	WCFrameDesc FrameDesc;
	
	int32 Height = 0;
	int32 Width = 0;
		
	if (!m_Session->GetFrameInfo(&FrameDesc))
	{
		return true;
	}

	Width = FrameDesc.width;
	Height = FrameDesc.height;

	if (Width != m_WindowSize.X || Height != m_WindowSize.Y)
	{
		m_WindowSize = FIntVector2D(Width, Height);

		ReCreateTexture();
		ChangeTexture.Broadcast(TextureTarget);
	}
		
	if (!m_Session->HasNewFrame()) return true;
			
	if (!TextureTarget || m_Session->m_buffer.Num() != Width * Height * 4)
	{
		return true;
	}

	FTextureResource* TextureResource = TextureTarget->GetResource();
	if (!TextureResource) return true;
	FRHITexture* RHITexture = TextureResource->GetTextureRHI();
	if (!RHITexture) return true;
	
	ENQUEUE_RENDER_COMMAND(UpdateTextureRegion)(
	[RHITexture, Width, Height, this](FRHICommandListImmediate& RHICmdList)
	{
		if (!RHITexture || _IsDisposed) {
			return;
		}

		m_Session->UseBuffer([this, Width, Height, &RHITexture, &RHICmdList]()
		{
			if (m_Session->m_buffer.Num() != Width * Height * 4)
			{
				return;
			}
				
			FUpdateTextureRegion2D Region(0, 0, 0, 0, Width, Height);
			RHICmdList.UpdateTexture2D(
				RHITexture,
				0,
				Region,
				Width * 4,
				m_Session->m_buffer.GetData()
			);
		});
	});

	return true;
}

bool UCaptureMachine::FindTargetWindow(HWND hWnd)
{
	__wchar_t windowTitle[1024];
	GetWindowText(hWnd, windowTitle, 1024);
	FString title(windowTitle);

	if (title.IsEmpty()) return true;

	bool isMatch = false;

	switch (Properties.TitleMatchingWindowSearch)
	{
	case ETitleMatchingWindowSearch::PerfectMatch:
		isMatch = title.Equals(Properties.CaptureTargetTitle, ESearchCase::IgnoreCase);
		break;

	case ETitleMatchingWindowSearch::ForwardMatch:
		isMatch = title.StartsWith(Properties.CaptureTargetTitle, ESearchCase::IgnoreCase);
		break;

	case ETitleMatchingWindowSearch::PartialMatch:
		isMatch = title.Contains(Properties.CaptureTargetTitle, ESearchCase::IgnoreCase);
		break;

	case ETitleMatchingWindowSearch::BackwardMatch:
		isMatch = title.EndsWith(Properties.CaptureTargetTitle, ESearchCase::IgnoreCase);
		break;

	case ETitleMatchingWindowSearch::RegularExpression:
		{
			const FRegexPattern pattern = FRegexPattern(Properties.CaptureTargetTitle);
			FRegexMatcher matcher(pattern, title);

			isMatch = matcher.FindNext();
		}
		break;
	}

	if (isMatch)
	{
		m_TargetWindow = hWnd;
		return false;
	}

	return true;
}

void UCaptureMachine::GetWindowSize()
{
	WCFrameDesc FrameDesc;
	if (!m_Session->GetFrameInfo(&FrameDesc))
	{
		WC_LOG(Log, "GetFrameInfo failed");
		return;
	}
	
	m_WindowSize = FIntVector2D(FrameDesc.width, FrameDesc.height);
}

void UCaptureMachine::ReCreateTexture()
{
	if (TextureTarget)
	{
		TextureTarget->ReleaseResource();
	}
	
	if (m_WindowSize.X == 0 || m_WindowSize.Y == 0)
	{
		TextureTarget = nullptr;
		return;
	}

	TextureTarget = UTexture2D::CreateTransient(m_WindowSize.X, m_WindowSize.Y, PF_B8G8R8A8);
	TextureTarget->UpdateResource();
}

#endif
