// Copyright 2019 ayumax. All Rights Reserved.
#ifdef PLATFORM_WINDOWS
#include "CaptureMachine.h"
#include "Engine/Texture2D.h"
#include "Async/Async.h"
#include "Internationalization/Regex.h"
#include "WindowCaptureSession.h"

#define WC_LOG(Format, ...) UE_LOG(LogTemp, Warning, TEXT("[WC][%p] %s: " Format), this, TEXT(__FUNCTION__), ##__VA_ARGS__)

UCaptureMachine::UCaptureMachine()
{
}

void UCaptureMachine::Start()
{
	m_Session = new WindowCaptureSession();

	CreateTexture();

	_TickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UCaptureMachine::TickCapture), 1.0f / (float)Properties.FrameRate);
}

void UCaptureMachine::Dispose()
{
	if (_IsDisposed)
	{
		return;
	}
	
	WC_LOG("[this=%p] Dispose called (_IsDisposed=%d)", this, _IsDisposed ? 1 : 0);
	
	if (_TickHandle.IsValid())
	{
		WC_LOG("[this=%p] RemoveTicker: before", this);
		FTSTicker::GetCoreTicker().RemoveTicker(_TickHandle);
		_TickHandle.Reset();
		WC_LOG("[this=%p] RemoveTicker: after", this);
	}

	if (m_Session)
	{
		m_Session->Stop();
		WC_LOG("[this=%p] Session Stop1", this);
		delete m_Session;
		WC_LOG("[this=%p] Session Stop2", this);
		m_Session = nullptr;
		WC_LOG("[this=%p] Stop: after", this);
	}
	
	if (TextureTarget)
	{
		WC_LOG("[this=%p] ReleaseResource TextureTarget: before", this);
		TextureTarget->ReleaseResource();
		TextureTarget = nullptr;
		WC_LOG("[this=%p] ReleaseResource TextureTarget: after", this);
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
	{
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
			
		if (TextureTarget && m_Session->m_buffer.Num() == Width * Height * 4)
		{
			if (FTextureResource* TextureResource = TextureTarget->GetResource())
			{
				if (FRHITexture* RHITexture = TextureResource->GetTextureRHI())
				{
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
				}
			}
		}		
	}

	return true;
}


UTexture2D* UCaptureMachine::CreateTexture()
{
	m_TargetWindow = nullptr;

	::EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
		{
			UCaptureMachine* my = (UCaptureMachine*)lParam;
			return my->FindTargetWindow(hwnd);
		}, (LPARAM)this);

	if (!m_TargetWindow) return nullptr;

	m_Session->Start(m_TargetWindow);

	GetWindowSize(m_TargetWindow);
	
	ReCreateTexture();
	
	return TextureTarget;	
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

void UCaptureMachine::GetWindowSize(HWND hWnd)
{
	if (!::IsWindow(hWnd))
	{
		m_WindowSize = FIntVector2D(0, 0);
		return;
	}

	WCFrameDesc FrameDesc;
	if (!m_Session->GetFrameInfo(&FrameDesc))
	{
		WC_LOG("[this=%p] GetFrameInfo failed", this);
		return;
	}
	
	m_WindowSize = FIntVector2D(FrameDesc.width, FrameDesc.height);
}

void UCaptureMachine::ReCreateTexture()
{
	if (m_WindowSize.X == 0 || m_WindowSize.Y == 0)
	{
		TextureTarget = nullptr;
		return;
	}

	TextureTarget = UTexture2D::CreateTransient(m_WindowSize.X, m_WindowSize.Y, PF_B8G8R8A8);
	TextureTarget->UpdateResource();
}

#endif
