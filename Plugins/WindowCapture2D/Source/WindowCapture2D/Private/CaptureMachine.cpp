// Copyright 2019 ayumax. All Rights Reserved.

#include "CaptureMachine.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/Texture2D.h"


ACaptureMachine::ACaptureMachine()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACaptureMachine::BeginPlay()
{
	Super::BeginPlay();
	
}

void ACaptureMachine::BeginDestroy()
{
	Super::BeginDestroy();

	if (m_hBmp)
	{
		DeleteObject(m_hBmp);
		m_BitmapBuffer = nullptr;
	}
}

void ACaptureMachine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!TextureTarget) return;

	PrintWindow(m_TargetWindow, m_MemDC, 2);

	UpdateTexture();
}

UTexture2D* ACaptureMachine::CreateTexture()
{
	m_TargetWindow = nullptr;

	EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL
		{
			ACaptureMachine* my = (ACaptureMachine*)lParam;
			__wchar_t windowTitle[1024];
			GetWindowText(hwnd, windowTitle, 1024);
			FString title(windowTitle);
			if (title.StartsWith(my->CaptureTargetTitle, ESearchCase::IgnoreCase))
			{
				my->m_TargetWindow = hwnd;
				return false;
			}

			return true;
		}, (LPARAM)this);

	if (!m_TargetWindow) return nullptr;

	auto targetWindowSize = GetWindowSize(m_TargetWindow);
	HDC foundDC = GetDC(m_TargetWindow);
	m_MemDC = CreateCompatibleDC(foundDC);

	ReleaseDC(m_TargetWindow, foundDC);

	ReCreateTexture(targetWindowSize);

	return TextureTarget;
}

void ACaptureMachine::UpdateTexture()
{
	if (!TextureTarget) return;

	auto Region = new FUpdateTextureRegion2D(0, 0, 0, 0, TextureTarget->GetSizeX(), TextureTarget->GetSizeY());
	TextureTarget->UpdateTextureRegions(0, 1, Region, 4 * TextureTarget->GetSizeX(), 4, (uint8*)m_BitmapBuffer);
}

FIntVector2D ACaptureMachine::GetWindowSize(HWND hWnd)
{
	RECT rect;
	GetWindowRect(hWnd, &rect);

	int32 width = rect.right - rect.left;
	int32 height = rect.bottom - rect.top;

	return FIntVector2D(width, height);
}

void ACaptureMachine::ReCreateTexture(FIntVector2D Size)
{
	if (m_hBmp)
	{
		DeleteObject(m_hBmp);
		m_BitmapBuffer = nullptr;
	}

	m_BitmapBuffer = new char[Size.X * Size.Y * 4];

	TextureTarget = UTexture2D::CreateTransient(Size.X, Size.Y, PF_B8G8R8A8);
	TextureTarget->UpdateResource();

	BITMAPINFO bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = Size.X;
	bmpInfo.bmiHeader.biHeight = Size.Y;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB;

	m_hBmp = CreateDIBSection(NULL, &bmpInfo, DIB_RGB_COLORS, (void**)&m_BitmapBuffer, NULL, 0);

	SelectObject(m_MemDC, m_hBmp);
}