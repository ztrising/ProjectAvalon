/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonHUD.h"
#include "../AvalonGame.h"
#include "../Gameplay/AvalonGameState.h" // Load game
#include "../../AvalonInput/AvalonInputManager.h" // Listening to input
#include "../Gameplay/Actor/PlayerActor.h" // Listening for travel
#include "../Gameplay/Components/TravellerComponent.h"

#include "../../AvalonViewport/FrameBuffer/FrameBuffer.h" // Initing draw buffer

#include "Widget_Background.h" // The base widget

#define AVALON_HUD_DEBUG 1

/*static*/ void AvalonHUD::InitializeHUD(FCoord ViewportSize)
{
	AvalonHUD& TheHUD = GetHUD();

	// The absolute draw buffer we collapse all widget draw buffers onto
	TheHUD.mBuffer = new FFrameBuffer();
	TheHUD.mBuffer->AllocateBuffer(ViewportSize, 'H', 0);
	FFrameBuffer::InitDrawBuffer(TheHUD.mBuffer);

	TheHUD.mBaseWidget = AvalonMemory::NewUnit<Widget_Background>();
	AvalonWidget* Widget = TheHUD.mBaseWidget.Get<AvalonWidget>();
	Widget->Construct();

	// The debug draw buffer we collapse onto the HUD draw buffer at the end
	// of the render step so we can display debug on top

#if AVALON_HUD_DEBUG
	TheHUD.mDebugBuffer = new FFrameBuffer();
	TheHUD.mDebugBuffer->AllocateBuffer(ViewportSize, AVALON_CHAR_TRANSPARENT, 0);
	FFrameBuffer::InitDebugDrawBuffer(TheHUD.mDebugBuffer);
#endif //AVALON_HUD_DEBUG
}

AvalonHUD::AvalonHUD()
{
	InputKeyEvent::Callback KeyCallback = AvalonHUD::HandleInputKeyEvent;
	AvalonInputManager::GetInputManager().mOnKeyStateChange.BindEvent(this, KeyCallback);

	InputMouseEvent::Callback MouseCallback = AvalonHUD::HandleInputMouseEvent;
	AvalonInputManager::GetInputManager().mOnMouseStateChange.BindEvent(this, MouseCallback);
}

AvalonHUD::~AvalonHUD()
{
	AvalonInputManager& InputManager = AvalonInputManager::GetInputManager();
	InputManager.mOnKeyStateChange.UnBindEvent(this);
	InputManager.mOnMouseStateChange.UnBindEvent(this);
}

void AvalonHUD::SetHUDContext(HUDContext NewContext)
{
	if (NewContext != mHUDContext)
	{
		mHUDContext = NewContext;
		mBaseWidget.Get<AvalonWidget>()->OnHUDContextChanged(mHUDContext);
	}
}

void AvalonHUD::HandleGameLoaded()
{
	if (PlayerActor* Player = PlayerActor::mPlayer)
	{
		if (Traveller* TravellerComp = Player->GetComponent<Traveller>())
		{
			Traveller::TravelEvent::Callback Callback = AvalonHUD::OnStartTravel;
			TravellerComp->mOnTravelBegin.BindEvent(this, Callback);

			Callback = AvalonHUD::OnFinishTravel;
			TravellerComp->mOnTravelEnd.BindEvent(this, Callback);
		}
	}

	mBaseWidget.Get<AvalonWidget>()->OnGameLoaded();

	SetHUDContext(EHUDContext::LOCATION);
}

void AvalonHUD::TickAnimation(float DeltaSeconds)
{
	mBaseWidget.Get<AvalonWidget>()->TickAnimation(DeltaSeconds);
}

void AvalonHUD::UpdateRenderState(bool& ForceRedraw)
{
	AvalonWidget* BaseWidget = mBaseWidget.Get<AvalonWidget>();
	BaseWidget->UpdateRenderState(ForceRedraw);

	//BaseWidget->DrawDebugText("X", mCachedMousePos);


	if (mWantsDebugBufferClear)
	{
		mDebugBuffer->FillBuffer(AVALON_CHAR_TRANSPARENT);
		mWantsDebugBufferClear = false;
		mDebugBufferChanged = true;
	}

	if (mDebugBufferChanged)
	{
		FFrameBuffer::CollapseBuffersDown(*mDebugBuffer, *mBuffer);
		mDebugBufferChanged = false;
	}
}

/***************************************************************************************
*  IEventListener
****************************************************************************************/
/*static*/ void AvalonHUD::HandleInputKeyEvent( IEventListener* Listener
											  , const FInputKeyEventParams& EventParams)
{
	AvalonHUD* HUD = static_cast<AvalonHUD*>(Listener);

	// Hack until we have an exit flow
	if (EventParams.mKeyDown)
	{
		if (EventParams.mEvent == EInputEvent::EQuit)
		{
			AvalonGame::GetGameInstance().SetsWantsExit();
			return;
		}

		if (EventParams.mEvent == EInputEvent::ELoadGame)
		{
			AvalonGameState::GetGameState().LoadGame();
			return;
		}

		if (EventParams.mEvent == EInputEvent::ENewGame)
		{
			AvalonGameState::GetGameState().NewGame();
			return;
		}
	}

	AvalonWidget* BaseWidget = HUD->mBaseWidget.Get<AvalonWidget>();
	BaseWidget->HandleInput_Internal(EventParams);
}

/*static*/ void AvalonHUD::HandleInputMouseEvent( IEventListener* Listener
												, const FInputMouseEventParams& EventParams)
{
	AvalonHUD* HUD = static_cast<AvalonHUD*>(Listener);

	HUD->mCachedMousePos = EventParams.mMousePosition;
	HUD->mDebugBufferChanged = true;

	AvalonWidget* BaseWidget = HUD->mBaseWidget.Get<AvalonWidget>();
	FUnitHandle NewFocus = BaseWidget->UpdateFocus(EventParams.mMousePosition);

	if (NewFocus != HUD->mFocusWidget)
	{
		if (HUD->mFocusWidget.IsValid())
		{
			if (AvalonWidget* Widget = HUD->mFocusWidget.Get<AvalonWidget>())
			{
				Widget->mHasFocus = false;
				Widget->OnFocusLost();
			}
		}

		HUD->mFocusWidget = NewFocus;

		if (HUD->mFocusWidget.IsValid())
		{
			if (AvalonWidget* Widget = HUD->mFocusWidget.Get<AvalonWidget>())
			{
				Widget->mHasFocus = true;
				Widget->OnFocusGained();
			}
		}
	}
}

/*static*/ void AvalonHUD::OnStartTravel( IEventListener* Listener
										, Traveller* InTraveller)
{
	AvalonHUD* HUD = static_cast<AvalonHUD*>(Listener);
	HUD->SetHUDContext(EHUDContext::TRAVELLING);
}

/*static*/ void AvalonHUD::OnFinishTravel( IEventListener* Listener
										 , Traveller* InTraveller)
{
	AvalonHUD* HUD = static_cast<AvalonHUD*>(Listener);
	HUD->SetHUDContext(EHUDContext::LOCATION);
}
/****************************************************************************************/
