/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_LocationPanel.h"
#include "Widget_Button.h";
#include "Widget_LocationOption.h"

#include "../Gameplay/Actions/ActionManager.h"
#include "../Gameplay/Map/TravelTypes.h"
#include "../Gameplay/Actor/LevelActor.h"
#include "../Gameplay/Actor/PlayerActor.h"
#include "../Gameplay/Components/TravellerComponent.h"
#include "../Gameplay/Components/ContainerComponent.h"
#include "../Gameplay/AvalonGameState.h"

Widget_LocationPanel::Widget_LocationPanel()
{
	mStartVisible = false;
	mValidHUDContexts = EHUDContext::LOCATION;
}

Widget_LocationPanel::~Widget_LocationPanel()
{
	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	if (Player)
	{
		if (Traveller* TravellerComp = Player->GetComponent<Traveller>())
		{
			TravellerComp->mOnTravelEnd.UnBindEvent(this);
		}

		LooterComponent* Looter = Player->GetComponent<LooterComponent>();
		if (Looter)
		{
			Looter->mOnContainerOpened.UnBindEvent(this);
			Looter->mOnContainerClosed.UnBindEvent(this);
		}
	}

	LevelActor* Level = AvalonGameState::GetCurrentLevel();
	if (Level)
	{
		Level->mOnItemAdded.UnBindEvent(this);
		Level->mOnItemRemoved.UnBindEvent(this);
	}

	//RemoveChild<AvalonWidget>(mTitleText);
	RemoveChild<AvalonWidget>(mDescTextRef);
}

void Widget_LocationPanel::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct("W_LocPanel_Frame.xml");

	//mTitleText	= AddChild<AvalonWidget>("W_LocPanel_Title_TextArea.xml");
	mDescTextRef = AddChild<AvalonWidget>("W_LocPanel_Desc_TextArea.xml");
}

void Widget_LocationPanel::OnGameLoaded()
{
	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	if (Player)
	{
		if (Traveller* TravellerComp = Player->GetComponent<Traveller>())
		{
			Traveller::TravelEvent::Callback Callback = Widget_LocationPanel::OnPlayerFinishTravel;
			TravellerComp->mOnTravelEnd.BindEvent(this, Callback);
		}

		LooterComponent::LooterEvent::Callback ContainerCallback = Widget_LocationPanel::HandleContainerOpened;
		LooterComponent* Looter = Player->GetComponent<LooterComponent>();
		Looter->mOnContainerOpened.BindEvent(this, ContainerCallback);

		ContainerCallback = Widget_LocationPanel::HandleContainerClosed;
		Looter->mOnContainerClosed.BindEvent(this, ContainerCallback);
	}

	LevelActor* Level = AvalonGameState::GetCurrentLevel();
	if (Level)
	{
		AvalonActor::InventoryEvent::Callback ItemCallback = Widget_LocationPanel::HandleLevelActorsChanged;
		Level->mOnItemAdded.BindEvent(this, ItemCallback);
		Level->mOnItemRemoved.BindEvent(this, ItemCallback);
	}

	AvalonWidget::OnGameLoaded();

	Show();
}

void Widget_LocationPanel::Show()
{
	AvalonWidget::Show();
	PopulateFromLocation();
}

void Widget_LocationPanel::PopulateFromLocation()
{
	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	LevelActor* PlayerLevel = Player->GetLevel();

	std::string LevelDescription = PlayerLevel->mDescription;

	AvalonWidget* Desc = Get<AvalonWidget>(mDescTextRef);
	Desc->SetTextBox(LevelDescription.c_str(), FCoord(3, 2), FCoord(34, 3), false, true);

	FBufferAnimSettings TextAnim = FBufferAnimSettings();
	TextAnim.mStyle = EAnimationStyle::FadeIn;
	TextAnim.mDirection = EAnimationDirection::DrizzleDown;
	TextAnim.mLength = 3.f;

	//Title->PlayAnimation(TextAnim);

	TextAnim.mLength = 10.f;
	//Desc->PlayAnimation(TextAnim);

	// Points of Interest
	unsigned int NumPoints = mLocationActors.size();
	for (unsigned int Index = 0; Index < NumPoints; ++Index)
	{
		RemoveChild<Widget_LocationOption>(mLocationActors[Index]);
	}
	mLocationActors.clear();

	FCoord ButtonPos = FCoord(0, 11);
	FCoord TextPos = FCoord(0, 0);
	FCoord TextExtent = FCoord(40, 4);

	HardRefList ActorsInLevel;
	PlayerLevel->GetActorsPlacedInLevel(ActorsInLevel);

	NumPoints = ActorsInLevel.size();
	for (unsigned int Index = 0; Index < NumPoints; ++Index)
	{
		HardUnitRef ActorRef = ActorsInLevel[Index];
		AvalonActor* Actor = Get<AvalonActor>(ActorRef);
		if (Actor != Player)
		{
			HardUnitRef NewButton = AddChild<Widget_LocationOption>();
			mLocationActors.push_back(NewButton);

			Widget_LocationOption* Button = Get<Widget_LocationOption>(NewButton);
			Button->SetLocationActorRef(ActorRef);

			Button->SetPosition(ButtonPos);
			ButtonPos.Y += 5;
		}
	}
}

void Widget_LocationPanel::PopulateTravelling()
{
	//AvalonWidget* Title = mTitleText.Get<AvalonWidget>();
	//Title->SetTextBox("Travelling..", FCoord(3, 2), FCoord(11, 1));

	AvalonWidget* Desc = Get<AvalonWidget>(mDescTextRef);
	Desc->SetTextBox("", FCoord(3, 2), FCoord(34, 3), false, true);
}

// "Left/Right" Panel Test
void Widget_LocationPanel::MoveLeft()
{
	FCoord EndLocation = mPositionData.Offset;
	EndLocation.X -= 20;

	StopAnimations();
	FMoveAnimSettings MoveAnim;
	MoveAnim.mStartLocation = mPositionData.Offset;
	MoveAnim.mEndLocation = EndLocation;
	MoveAnim.mLength = 0.25f;
	MoveTo(&MoveAnim);
}

void Widget_LocationPanel::MoveRight()
{
	FCoord EndLocation = mPositionData.Offset;

	StopAnimations();
	FMoveAnimSettings MoveAnim;
	MoveAnim.mStartLocation = mPositionData.Offset;
	MoveAnim.mEndLocation = EndLocation;
	MoveAnim.mLength = 0.25f;
	MoveTo(&MoveAnim);
}

void Widget_LocationPanel::StartInspectingFloor(IContainer* Container)
{
	// TODO:
	// Show Floor Container
	// MoveLeft
}

void Widget_LocationPanel::StopInspectingFloor(IContainer* Container)
{
	// TODO:
	// Hide Floor Container
	// MoveRight
}

/*static*/ void Widget_LocationPanel::OnPlayerFinishTravel( IAvalonUnit* Listener
														  , Traveller* PlayerTraveller)
{
	if (Widget_LocationPanel* Widget = static_cast<Widget_LocationPanel*>(Listener))
	{
		Widget->PopulateFromLocation();
	}
}

/*static*/ void Widget_LocationPanel::HandleLevelActorsChanged( IAvalonUnit* Listener
															  , AvalonActor* Item)
{
	Widget_LocationPanel* Widget = static_cast<Widget_LocationPanel*>(Listener);
	Widget->PopulateFromLocation();
}

/*static*/ void Widget_LocationPanel::HandleContainerOpened( IAvalonUnit* Listener
														   , IContainer* Container)
{
	Widget_LocationPanel* Widget = static_cast<Widget_LocationPanel*>(Listener);
	Widget->StartInspectingFloor(Container);
}

/*static*/ void Widget_LocationPanel::HandleContainerClosed( IAvalonUnit* Listener
														   , IContainer* Container)
{
	Widget_LocationPanel* Widget = static_cast<Widget_LocationPanel*>(Listener);
	Widget->StopInspectingFloor(Container);
}