/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_Inventory.h"

#include "Widget_Button.h"
#include "Widget_ItemPouch.h"
#include "Widget_ItemContainer.h"

#include "../Gameplay/Actor/PlayerActor.h"
#include "../Gameplay/Actor/ActorTypes.h"
#include "../Gameplay/Components/ContainerComponent.h"
#include "../Gameplay/Components/TravellerComponent.h"
#include "../Gameplay/AvalonGameState.h"


Widget_Inventory::Widget_Inventory()
{
	mStartVisible = false;
}

Widget_Inventory::~Widget_Inventory()
{
	// TODO: Unbind, Remove Child
}

void Widget_Inventory::OnGameLoaded()
{
	PopulatePouches();

	if (AvalonActor* Player = AvalonGameState::GetPlayerActor())
	{
		if (Traveller* TravellerComp = Player->GetComponent<Traveller>())
		{
			Traveller::TravelEvent::Callback Callback = Widget_Inventory::OnPlayerStartTravel;
			TravellerComp->mOnTravelBegin.BindEvent(this, Callback);
		}

		AvalonActor::InventoryEvent::Callback Callback = Widget_Inventory::HandleItemAdded;
		Player->mOnItemAdded.BindEvent(this, Callback);

		Callback = Widget_Inventory::HandleItemRemoved;
		Player->mOnItemRemoved.BindEvent(this, Callback);

		LooterComponent::LooterEvent::Callback ContainerCallback = Widget_Inventory::HandleContainerOpened;
		LooterComponent* Looter = Player->GetComponent<LooterComponent>();
		Looter->mOnContainerOpened.BindEvent(this, ContainerCallback);

		ContainerCallback = Widget_Inventory::HandleContainerClosed;
		Looter->mOnContainerClosed.BindEvent(this, ContainerCallback);
	}

	Show();

	AvalonWidget::OnGameLoaded();
}

void Widget_Inventory::RequestContainerOpen(IContainer* Container)
{
	AvalonActor* ContainerActor = Container->GetActorOwner();
	AvalonActor* PlayerActor = AvalonGameState::GetPlayerActor();
	if (ContainerActor->IsOwnedBy(PlayerActor))
	{
		// Have to find the correct pouch to "Activate"
		auto PouchIt = mItemPouches.begin();
		auto ContainerIt = mItemPouchContainers.begin();

		while (ContainerIt != mItemPouchContainers.end())
		{
			Widget_ItemContainer* ContainerWidget = Get<Widget_ItemContainer>((*ContainerIt));
			if (ContainerWidget->GetContainer() == Container)
			{
				Widget_ItemPouch* PouchWidget = Get<Widget_ItemPouch>((*PouchIt));
				PouchWidget->SetActive(true);
				HandleButtonPressed(this, PouchWidget);
				return;
			}

			PouchIt++;
			ContainerIt++;
		}
	}
	else
	{
		OpenLevelContainer(Container);
	}
}

void Widget_Inventory::RequestContainerClosed(IContainer* Container)
{
	AvalonActor* ContainerActor = Container->GetActorOwner();
	AvalonActor* PlayerActor = AvalonGameState::GetPlayerActor();
	if (ContainerActor->IsOwnedBy(PlayerActor))
	{
		// Have to find the correct pouch to "Activate"
	}
	else
	{
		CloseLevelContainer();
	}
}

void Widget_Inventory::OpenLevelContainer(IContainer* Container)
{
	Widget_ItemContainer* Widget = Get<Widget_ItemContainer>(mLevelContainer);
	ItemContainer* ItemCont = (ItemContainer*)(Container);
	Widget->SetContainer(ItemCont);

	FMoveAnimSettings MoveAnim;
	MoveAnim.mStartLocation = FCoord(-90, 0);
	MoveAnim.mEndLocation = FCoord(-90, -5);
	MoveAnim.mLength = 0.25f;
	Widget->MoveTo(&MoveAnim);

	Widget->Show();
	//mCloseButton.Get<AvalonWidget>()->Show();
}

void Widget_Inventory::CloseLevelContainer()
{
	if (mCloseAnimation == nullptr)
	{
		Widget_ItemContainer* ContainerWidget = Get<Widget_ItemContainer>(mLevelContainer);
		ContainerWidget->SetContainer(nullptr);

		FMoveAnimSettings MoveAnim;
		MoveAnim.mStartLocation = FCoord(-90, -5);
		MoveAnim.mEndLocation = FCoord(-90, 0);
		MoveAnim.mLength = 0.25f;

		mCloseAnimation = ContainerWidget->MoveTo(&MoveAnim);
		IAvalonAnimation::AnimEvent::Callback Callback = Widget_Inventory::OnCloseAnimFinished;
		mCloseAnimation->mOnAnimEnded.BindEvent(this, Callback);
	}
}

void Widget_Inventory::HideLevelContainer()
{
	mCloseAnimation = nullptr;

	Widget_ItemContainer* ContainerWidget = Get<Widget_ItemContainer>(mLevelContainer);
	ContainerWidget->Hide();

	//mCloseButton.Get<AvalonWidget>()->Hide();
}

void Widget_Inventory::PopulatePouches()
{
	for (auto& Handle : mItemPouches)
	{
		RemoveChild<Widget_ItemPouch>(Handle);
	}
	mItemPouches.clear();

	for (auto& Handle : mItemPouchContainers)
	{
		RemoveChild<Widget_ItemContainer>(Handle);
	}
	mItemPouchContainers.clear();

	std::vector<IContainer*> PlayerPouches;

	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	Player->GetItemPouchContainers(PlayerPouches);

	FCoord Pos(3, 0);
	FCoord ContainerOffset(3, 0);

	for (IContainer* Container : PlayerPouches)
	{
		ItemContainer* ItemCont = (ItemContainer*)(Container);

		HardUnitRef ItemPouch = AddChild<Widget_ItemPouch>();
		mItemPouches.push_back(ItemPouch);

		Widget_Button* Button = Get<Widget_Button>(ItemPouch);
		Widget_Button::ButtonEvent::Callback ButtonPressedCallback = Widget_Inventory::HandleButtonPressed;
		Button->mOnButtonPressed.BindEvent(this, ButtonPressedCallback);

		Button->SetPosition(Pos);

		HardUnitRef ContainerHandle = AddChild<Widget_ItemContainer>("W_ItemPanel_Frame.xml");
		mItemPouchContainers.push_back(ContainerHandle);

		Widget_ItemContainer* ContainerWidget = Get<Widget_ItemContainer>(ContainerHandle);
		ContainerWidget->Hide();
		ContainerWidget->SetContainer(ItemCont);
		ContainerWidget->SetPosition(ContainerOffset);

		// TODO:  Y Offsets!
		Pos.Y -= 10;
		ContainerOffset.Y -= 11;
	}

	// Amy Testing Something
	CloseLevelContainer();
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_Inventory::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct("W_Inventory_Frame.xml");

	mLevelContainer = AddChild<Widget_ItemContainer>("W_ItemPanel_Level.xml");

	Widget_ItemContainer* Widget = Get<Widget_ItemContainer>(mLevelContainer);
	Widget->Hide();

	//mCloseButton = mLevelContainer.Get<AvalonWidget>()->AddChild<Widget_Button>("W_Journal_Button.xml");
	//Widget_Button::ButtonEvent::Callback ButtonPressedCallback = Widget_Inventory::HandleButtonPressed;
	//mCloseButton.Get<Widget_Button>()->mOnButtonPressed.BindEvent(this, ButtonPressedCallback);
	//mCloseButton.Get<Widget_Button>()->SetPosition(FCoord(20,0));


	Widget->SetPosition(FCoord(-90, 0));
}

void Widget_Inventory::Show()
{
	AvalonWidget::Show();

	PopulatePouches();
}

void Widget_Inventory::Hide()
{
	//AvalonInventory::GetInventory().SetItemFocus(nullptr);
	AvalonWidget::Hide();
}
/****************************************************************************************/

/***************************************************************************************
*  IEventListener
****************************************************************************************/

/*static*/ void Widget_Inventory::HandleItemAdded( IEventListener* Listener
												 , AvalonActor* AddedItem)
{
	Widget_Inventory* Widget = static_cast<Widget_Inventory*>(Listener);
	Widget->PopulatePouches();
}

/*static*/ void Widget_Inventory::HandleItemRemoved( IEventListener* Listener
												   , AvalonActor* RemovedItem)
{
	Widget_Inventory* Widget = static_cast<Widget_Inventory*>(Listener);
	Widget->PopulatePouches();
}

/*static*/ void Widget_Inventory::HandleContainerOpened( IEventListener* Listener
													   , IContainer* Container)
{
	Widget_Inventory* Widget = static_cast<Widget_Inventory*>(Listener);
	Widget->RequestContainerOpen(Container);
}

/*static*/ void Widget_Inventory::HandleContainerClosed( IEventListener* Listener
													   , IContainer* Container)
{
	Widget_Inventory* Widget = static_cast<Widget_Inventory*>(Listener);
	Widget->RequestContainerClosed(Container);
}

/*static*/ void Widget_Inventory::OnPlayerStartTravel( IEventListener* Listener
													 , Traveller* Traveller)
{
	Widget_Inventory* Widget = static_cast<Widget_Inventory*>(Listener);
	Widget->CloseLevelContainer();
}

/*static*/ void Widget_Inventory::OnCloseAnimFinished( IEventListener* Listener
													 , IAvalonAnimation* Animation)
{
	Widget_Inventory* Widget = static_cast<Widget_Inventory*>(Listener);
	Widget->HideLevelContainer();
}

/*static*/ void Widget_Inventory::HandleButtonPressed( IEventListener* Listener
													 , const Widget_Button* Source)
{
	Widget_Inventory* Widget = static_cast<Widget_Inventory*>(Listener);

	//if (Source == Widget->mCloseButton)
	//{
		Widget->CloseLevelContainer();
	//	return;
	//}

	for (int i = 0; i < Widget->mItemPouches.size(); ++i)
	{
		if (Widget->mItemPouches[i].get() == Source)
		{
			HardUnitRef PouchRef = Widget->mItemPouches[i];
			Widget_ItemPouch* Pouch = Get<Widget_ItemPouch>(Widget->mItemPouches[i]);
			Widget_ItemContainer* Container = Get<Widget_ItemContainer>(Widget->mItemPouchContainers[i]);

			if (Pouch->GetActive())
			{
				Pouch->SetActive(true);
				Container->PopulateItems();
				Container->Show();
			}
			else
			{
				//AvalonInventory::GetInventory().SetItemFocus(nullptr);
				Container->Hide();
			}

			return;
		}
	}
}
/****************************************************************************************/
