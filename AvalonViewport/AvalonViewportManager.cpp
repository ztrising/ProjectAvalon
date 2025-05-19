/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonViewportManager.h"
#include "AvalonViewport.h"
#include "ViewportElement.h"
#include "../AvalonGame/Widgets/AvalonHUD.h"

#include "WindowsPlatform/AvalonViewport_WindowsPlatform.h"

AvalonViewportManager::~AvalonViewportManager()
{
	Teardown();
}

void AvalonViewportManager::Initialize()
{


	mCurrentViewport = new AvalonViewport_WindowsPlatform();
	//mCurrentViewport->SetFullscreen();
	mCurrentViewport->InitializeViewport();
	FCoord ViewportSize = mCurrentViewport->GetViewportSize();

	AvalonHUD::InitializeHUD(ViewportSize);

	mForceRedraw = true;

	// Log window test
	//mCurrentViewport->CreateLogWindow();
}

void AvalonViewportManager::Teardown()
{
	mCurrentViewport->TeardownViewport();
	delete mCurrentViewport;
}

FCoord AvalonViewportManager::GetViewportSize()
{
	return (mCurrentViewport) ? mCurrentViewport->GetViewportSize() : FCoord();
}

void AvalonViewportManager::Draw(float DeltaSeconds)
{
	AvalonHUD& HUD = AvalonHUD::GetHUD();
	if (mForceRedraw)
	{
		HUD.mBuffer->FillBuffer();
	}

	AvalonHUD::GetHUD().TickAnimation(DeltaSeconds);
	AvalonHUD::GetHUD().UpdateRenderState(mForceRedraw);

	// Any changes to the frame require a full redraw
	// technically we can improve this by calculating
	// a sub buffer where the changes were actually
	// made...
	if (mForceRedraw)
	{
		mCurrentViewport->DrawArea(*(AvalonHUD::GetHUD().mBuffer));
	}

	mForceRedraw = false;
}


void* AvalonViewportManager::GetInputHandle()
{
	return mCurrentViewport->GetInputHandle();
}

