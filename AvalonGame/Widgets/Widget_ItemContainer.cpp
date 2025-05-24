/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_ItemContainer.h"
#include "Widget_ToggleButton.h"
#include "Widget_List.h"

#include "../Gameplay/Components/ContainerComponent.h"
#include "../Gameplay/Actor/AvalonActor.h"
#include "../Gameplay/Actions/ActionManager.h"
#include "../Gameplay/Actor/ActorTypes.h"

#include <string>

Widget_ItemContainer::~Widget_ItemContainer()
{
	SetContainer(nullptr);

	Widget_List* List = Get<Widget_List>(mItemButtonListRef);
	List->EmptyList<Widget_ToggleButton>();

	RemoveChild<Widget_List>(mItemButtonListRef);
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_ItemContainer::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct(WidgetAsset);
	mItemButtonListRef = AddChild<Widget_List>();
	Widget_List* List = Get<Widget_List>(mItemButtonListRef);
	List->SetPosition(FCoord(3, 3));
}
/****************************************************************************************/

void Widget_ItemContainer::SetContainer(ItemContainer* NewItemContainer)
{
	if (mItemContainerRef != nullptr)
	{
		ItemContainer* OldItemContainer = Get<ItemContainer>(mItemContainerRef);

		// Unbind event
		AvalonActor* ParentActor = OldItemContainer->GetActorOwner();
		if (ParentActor)
		{
			ParentActor->mOnItemAdded.UnBindEvent(this);
			ParentActor->mOnItemRemoved.UnBindEvent(this);
		}
	}

	HardUnitRef NewItemRef;
	if (NewItemContainer != nullptr)
	{
		NewItemRef = NewItemContainer->GetSelfRef();
	}

	mItemContainerRef = NewItemRef;

	if (mItemContainerRef != nullptr)
	{
		ItemContainer* NewItemContainer = Get<ItemContainer>(mItemContainerRef);

		// Bind event
		AvalonActor* ParentActor = NewItemContainer->GetActorOwner();
		AvalonActor::InventoryEvent::Callback Callback = Widget_ItemContainer::HandleContentsChanged;
		ParentActor->mOnItemAdded.BindEvent(this, Callback);
		ParentActor->mOnItemRemoved.BindEvent(this, Callback);

		PopulateItems();
	}
}

void Widget_ItemContainer::PopulateItems()
{
	if (mItemContainerRef == nullptr)
	{
		return;
	}

	ItemContainer* Container = Get<ItemContainer>(mItemContainerRef);
	unsigned int NumItems = Container->GetNumItems();

	/***************************************************************************************
	*  Container Name and Capacity Display
	****************************************************************************************/
	FTextSettings TextSettings;
	TextSettings.mOffset = FCoord(0,1);
	TextSettings.mExtent = FCoord(20,1);
	//TextSettings.mUseBufferExtentWidth = true;
	TextSettings.mJustified = ETextJustification::Left;
	std::string DisplayName = Container->GetDisplayName();

	SetTextWithSettings(DisplayName.c_str(), TextSettings);
	
	// Show capacity if it isn't infinite
	int MaxSlots = Container->GetMaxSlots();
	if (MaxSlots > 0)
	{
		TextSettings.mOffset = FCoord(-1,1);
		TextSettings.mExtent = FCoord(0,1);
		TextSettings.mUseBufferExtentWidth = true;
		TextSettings.mJustified = ETextJustification::Right;

		std::string Header = "[" + std::to_string(NumItems) + "/" + std::to_string(MaxSlots) + "]";
		SetTextWithSettings(Header.c_str(), TextSettings);
	}
	/****************************************************************************************/

	/***************************************************************************************
	*  Item List - Create Buttons, Populate Names, Listen for Input
	****************************************************************************************/
	Widget_List* List = Get<Widget_List>(mItemButtonListRef);

	// Creates the buttons, and displays them based on settings
	FWidgetListSettings ListSettings;
	List->ConstructList<Widget_ToggleButton>(NumItems, ListSettings, "W_ItemPanel_ItemEntry.xml");

	auto InitButtonLambda = [&](HardUnitRef& WidgetRef)
	{
		int TextIndex = List->GetElementIndex(WidgetRef);
		Widget_ToggleButton* Button = Get<Widget_ToggleButton>(WidgetRef);

		HardUnitRef ItemRef = Container->GetContentsByIndex(TextIndex);
		AvalonActor* Item = Get<AvalonActor>(ItemRef);
		if (Item != nullptr)
		{
			std::string DisplayName = Get<AvalonActor>(ItemRef)->mDisplayName;
			Button->SetText(DisplayName.c_str(), FCoord(0, 0));
			Widget_Button::ButtonEvent::Callback ButtonPressedCallback = Widget_ItemContainer::HandleButtonPressed;
			Button->mOnButtonPressed.BindEvent(this, ButtonPressedCallback);
		}
	};

	List->ForEachItem(InitButtonLambda);
	/****************************************************************************************/
}

void Widget_ItemContainer::DeselectAll()
{
	Widget_List* List = Get<Widget_List>(mItemButtonListRef);
	auto DeselectLambda = [&](HardUnitRef& WidgetRef)
	{
		Widget_ToggleButton* Button = Get<Widget_ToggleButton>(WidgetRef);
		Button->SetActive(false);
	};

	List->ForEachItem(DeselectLambda);
}

/***************************************************************************************
*  IEventListener
****************************************************************************************/
/*static*/ void Widget_ItemContainer::HandleContentsChanged( IAvalonUnit* Listener
														   , AvalonActor* Item)
{
	Widget_ItemContainer* Widget = static_cast<Widget_ItemContainer*>(Listener);
	Widget->PopulateItems();
}

/*static*/ void Widget_ItemContainer::HandleButtonPressed( IAvalonUnit* Listener
														 , const Widget_Button* Source)
{
	Widget_ItemContainer* ItemContainerWidget = static_cast<Widget_ItemContainer*>(Listener);
	const Widget_ToggleButton* FocusButton = static_cast<const Widget_ToggleButton*>(Source);

	bool SetFocus = FocusButton->GetActive();

	Widget_List* ListWidget = Get<Widget_List>(ItemContainerWidget->mItemButtonListRef);

	HardUnitRef WidgetButtonRef = Source->GetSelfRef();
	int FocusIndex = ListWidget->GetElementIndex(WidgetButtonRef);

	// Make sure only one Item is active at the same time
	if (SetFocus)
	{
		auto DeactiveNotFocusLamdba = [&](HardUnitRef& WidgetRef)
		{
			if (WidgetRef != WidgetButtonRef)
			{
				Widget_ToggleButton* Button = Get<Widget_ToggleButton>(WidgetRef);
				Button->SetActive(false);
			}
		};

		ListWidget->ForEachItem(DeactiveNotFocusLamdba);
	}

	// Tell the inventory to focus on the new item/container
	ItemContainer* Container = Get<ItemContainer>(ItemContainerWidget->mItemContainerRef);
	HardUnitRef ItemRef = Container->GetContentsByIndex(FocusIndex);
	if (ItemRef != nullptr)
	{
		ActionManager::Get().SetActionFocus(ItemRef);
	}
	else
	{
		ActionManager::Get().ClearActionFocus();
	}
}
/****************************************************************************************/

