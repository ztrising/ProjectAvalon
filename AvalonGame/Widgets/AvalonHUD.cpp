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

	AvalonMemory::NewUnit<Widget_Background>(TheHUD.mBaseWidget);
	AvalonWidget* Widget = IAvalonUnit::Get<AvalonWidget>(TheHUD.mBaseWidget);
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
		AvalonWidget::Get<AvalonWidget>(mBaseWidget)->OnHUDContextChanged(mHUDContext);
	}
}

void AvalonHUD::HandleGameLoaded()
{
	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	if (Player != nullptr)
	{
		if (Traveller* TravellerComp = Player->GetComponent<Traveller>())
		{
			Traveller::TravelEvent::Callback Callback = AvalonHUD::OnStartTravel;
			TravellerComp->mOnTravelBegin.BindEvent(this, Callback);

			Callback = AvalonHUD::OnFinishTravel;
			TravellerComp->mOnTravelEnd.BindEvent(this, Callback);
		}
	}

	AvalonWidget::Get<AvalonWidget>(mBaseWidget)->OnGameLoaded();

	SetHUDContext(EHUDContext::LOCATION);
}

void AvalonHUD::TickAnimation(float DeltaSeconds)
{
	AvalonWidget::Get<AvalonWidget>(mBaseWidget)->TickAnimation(DeltaSeconds);
}

void AvalonHUD::UpdateRenderState(bool& ForceRedraw)
{
	AvalonWidget::Get<AvalonWidget>(mBaseWidget)->UpdateRenderState(ForceRedraw);

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

	AvalonWidget::Get<AvalonWidget>(HUD->mBaseWidget)->HandleInput_Internal(EventParams);
}

/*static*/ void AvalonHUD::HandleInputMouseEvent( IEventListener* Listener
												, const FInputMouseEventParams& EventParams)
{
	AvalonHUD* HUD = static_cast<AvalonHUD*>(Listener);

	HUD->mCachedMousePos = EventParams.mMousePosition;
	HUD->mDebugBufferChanged = true;

	AvalonWidget* BaseWidget = AvalonWidget::Get<AvalonWidget>(HUD->mBaseWidget);
	AvalonWidget* NewFocus = BaseWidget->UpdateFocus(EventParams.mMousePosition);

	HardUnitRef FocusRef = HUD->mFocusWidget.lock();
	AvalonWidget* FocusWidget = AvalonWidget::Get<AvalonWidget>(FocusRef);

	if (NewFocus != FocusWidget)
	{
		if (FocusWidget != nullptr)
		{
			FocusWidget->mHasFocus = false;
			FocusWidget->OnFocusLost();
		}

		if (NewFocus != nullptr)
		{
			HUD->mFocusWidget = NewFocus->GetSelfRef();
			NewFocus->mHasFocus = true;
			NewFocus->OnFocusGained();
		}
		else
		{
			HUD->mFocusWidget.reset();
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
