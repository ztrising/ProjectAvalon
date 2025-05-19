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

	if (PlayerActor* Player = PlayerActor::mPlayer)
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
		LooterComponent* Looter = PlayerActor::mPlayer->GetComponent<LooterComponent>();
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
	if (ContainerActor->IsOwnedBy(PlayerActor::mPlayerHandle))
	{
		// Have to find the correct pouch to "Activate"
		auto PouchIt = mItemPouches.begin();
		auto ContainerIt = mItemPouchContainers.begin();

		while (ContainerIt != mItemPouchContainers.end())
		{
			Widget_ItemContainer* ContainerWidget = (*ContainerIt).Get<Widget_ItemContainer>();
			if (ContainerWidget->GetContainer() == Container)
			{
				Widget_ItemPouch* PouchWidget = (*PouchIt).Get<Widget_ItemPouch>();
				PouchWidget->SetActive(true);
				HandleButtonPressed(this, *PouchIt);
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
	if (ContainerActor->IsOwnedBy(PlayerActor::mPlayerHandle))
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
	Widget_ItemContainer* Widget = mLevelContainer.Get<Widget_ItemContainer>();
	ItemContainer* ItemCont = (ItemContainer*)(Container);
	Widget->SetContainer(ItemCont);

	FMoveAnimSettings MoveAnim;
	MoveAnim.mStartLocation = FCoord(-90, 0);
	MoveAnim.mEndLocation = FCoord(-90, -5);
	MoveAnim.mLength = 0.25f;
	Widget->MoveTo(MoveAnim);

	Widget->Show();
	//mCloseButton.Get<AvalonWidget>()->Show();
}

void Widget_Inventory::CloseLevelContainer()
{
	if (mCloseAnimation == nullptr)
	{
		Widget_ItemContainer* ContainerWidget = mLevelContainer.Get<Widget_ItemContainer>();
		ContainerWidget->SetContainer(nullptr);

		FMoveAnimSettings MoveAnim;
		MoveAnim.mStartLocation = FCoord(-90, -5);
		MoveAnim.mEndLocation = FCoord(-90, 0);
		MoveAnim.mLength = 0.25f;

		mCloseAnimation = ContainerWidget->MoveTo(MoveAnim);
		IAvalonAnimation::AnimEvent::Callback Callback = Widget_Inventory::OnCloseAnimFinished;
		mCloseAnimation->mOnAnimEnded.BindEvent(this, Callback);
	}
}

void Widget_Inventory::HideLevelContainer()
{
	mCloseAnimation = nullptr;

	Widget_ItemContainer* ContainerWidget = mLevelContainer.Get<Widget_ItemContainer>();
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
	PlayerActor::mPlayer->GetItemPouchContainers(PlayerPouches);

	FCoord Pos(3, 0);
	FCoord ContainerOffset(3, 0);

	for (IContainer* Container : PlayerPouches)
	{
		ItemContainer* ItemCont = (ItemContainer*)(Container);

		FUnitHandle ItemPouch = AddChild<Widget_ItemPouch>();
		mItemPouches.push_back(ItemPouch);

		Widget_Button* Button = (Widget_Button*)(ItemPouch.Get());
		Widget_Button::ButtonEvent::Callback ButtonPressedCallback = Widget_Inventory::HandleButtonPressed;
		Button->mOnButtonPressed.BindEvent(this, ButtonPressedCallback);

		Button->SetPosition(Pos);

		FUnitHandle ContainerHandle = AddChild<Widget_ItemContainer>("W_ItemPanel_Frame.xml");
		ContainerHandle.Get<AvalonWidget>()->Hide();
		mItemPouchContainers.push_back(ContainerHandle);

		Widget_ItemContainer* ContainerWidget = (Widget_ItemContainer*)(ContainerHandle.Get());
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
	mLevelContainer.Get<AvalonWidget>()->Hide();

	//mCloseButton = mLevelContainer.Get<AvalonWidget>()->AddChild<Widget_Button>("W_Journal_Button.xml");
	//Widget_Button::ButtonEvent::Callback ButtonPressedCallback = Widget_Inventory::HandleButtonPressed;
	//mCloseButton.Get<Widget_Button>()->mOnButtonPressed.BindEvent(this, ButtonPressedCallback);
	//mCloseButton.Get<Widget_Button>()->SetPosition(FCoord(20,0));


	mLevelContainer.Get<AvalonWidget>()->SetPosition(FCoord(-90, 0));
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
												 , FUnitHandle& AddedItem)
{
	Widget_Inventory* Widget = static_cast<Widget_Inventory*>(Listener);
	Widget->PopulatePouches();
}

/*static*/ void Widget_Inventory::HandleItemRemoved( IEventListener* Listener
												   , FUnitHandle& RemovedItem)
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
													 , const FUnitHandle& Source)
{
	Widget_Inventory* Widget = static_cast<Widget_Inventory*>(Listener);

	//if (Source == Widget->mCloseButton)
	//{
		Widget->CloseLevelContainer();
	//	return;
	//}

	for (int i = 0; i < Widget->mItemPouches.size(); ++i)
	{
		if (Widget->mItemPouches[i] == Source)
		{
			Widget_ItemPouch* Pouch = Widget->mItemPouches[i].Get<Widget_ItemPouch>();
			if (Pouch->GetActive())
			{
				Pouch->SetActive(true);
				Widget_ItemContainer* Container = Widget->mItemPouchContainers[i].Get<Widget_ItemContainer>();
				Container->PopulateItems();
				Container->Show();
			}
			else
			{
				//AvalonInventory::GetInventory().SetItemFocus(nullptr);
				Widget->mItemPouchContainers[i].Get<AvalonWidget>()->Hide();
			}

			return;
		}
	}
}
/****************************************************************************************/
