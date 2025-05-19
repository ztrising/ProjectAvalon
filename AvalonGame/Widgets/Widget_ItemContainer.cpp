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

	Widget_List* List = mItemButtonList.Get<Widget_List>();
	List->EmptyList<Widget_ToggleButton>();

	RemoveChild<Widget_List>(mItemButtonList);
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_ItemContainer::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct(WidgetAsset);
	mItemButtonList = AddChild<Widget_List>();
	mItemButtonList.Get<Widget_List>()->SetPosition(FCoord(3, 3));
}
/****************************************************************************************/

void Widget_ItemContainer::SetContainer(ItemContainer* NewContainer)
{
	if (mContainer != nullptr)
	{
		// Unbind event
		AvalonActor* Actor = mContainer->GetActorOwnerHandle().Get<AvalonActor>();
		Actor->mOnItemAdded.UnBindEvent(this);
		Actor->mOnItemRemoved.UnBindEvent(this);
	}

	mContainer = NewContainer;

	if (mContainer != nullptr)
	{
		// Bind event
		AvalonActor* Actor = mContainer->GetActorOwnerHandle().Get<AvalonActor>();
		AvalonActor::InventoryEvent::Callback Callback = Widget_ItemContainer::HandleContentsChanged;
		Actor->mOnItemAdded.BindEvent(this, Callback);
		Actor->mOnItemRemoved.BindEvent(this, Callback);

		PopulateItems();
	}
}

void Widget_ItemContainer::PopulateItems()
{
	if (mContainer == nullptr)
	{
		return;
	}

	unsigned int NumItems = mContainer->GetNumItems();

	/***************************************************************************************
	*  Container Name and Capacity Display
	****************************************************************************************/
	FTextSettings TextSettings;
	TextSettings.mOffset = FCoord(0,1);
	TextSettings.mExtent = FCoord(20,1);
	//TextSettings.mUseBufferExtentWidth = true;
	TextSettings.mJustified = ETextJustification::Left;
	std::string DisplayName = mContainer->GetDisplayName();

	SetTextWithSettings(DisplayName.c_str(), TextSettings);
	
	// Show capacity if it isn't infinite
	int MaxSlots = mContainer->GetMaxSlots();
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
	Widget_List* List = mItemButtonList.Get<Widget_List>();

	// Creates the buttons, and displays them based on settings
	FWidgetListSettings ListSettings;
	List->ConstructList<Widget_ToggleButton>(NumItems, ListSettings, "W_ItemPanel_ItemEntry.xml");

	auto InitButtonLambda = [&](FUnitHandle& Handle)
	{
		int TextIndex = List->GetElementIndex(Handle);
		Widget_ToggleButton* Button = Handle.Get<Widget_ToggleButton>();

		FUnitHandle Item = mContainer->GetContentsByIndex(TextIndex);
		std::string DisplayName = Item.Get<AvalonActor>()->mDisplayName;

		Button->SetText(DisplayName.c_str(), FCoord(0, 0));
		Widget_Button::ButtonEvent::Callback ButtonPressedCallback = Widget_ItemContainer::HandleButtonPressed;
		Button->mOnButtonPressed.BindEvent(this, ButtonPressedCallback);
	};

	List->ForEachItem(InitButtonLambda);
	/****************************************************************************************/
}

void Widget_ItemContainer::DeselectAll()
{
	Widget_List* List = mItemButtonList.Get<Widget_List>();
	auto DeselectLambda = [&](FUnitHandle& Handle)
	{
		Widget_ToggleButton* Button = Handle.Get<Widget_ToggleButton>();
		Button->SetActive(false);
	};

	List->ForEachItem(DeselectLambda);
}

/***************************************************************************************
*  IEventListener
****************************************************************************************/
/*static*/ void Widget_ItemContainer::HandleContentsChanged( IEventListener* Listener
														   , FUnitHandle& Item)
{
	Widget_ItemContainer* Widget = static_cast<Widget_ItemContainer*>(Listener);
	Widget->PopulateItems();
}

/*static*/ void Widget_ItemContainer::HandleButtonPressed( IEventListener* Listener
														 , const FUnitHandle& Source)
{
	Widget_ItemContainer* Widget = static_cast<Widget_ItemContainer*>(Listener);

	Widget_ToggleButton* FocusButton = Source.Get<Widget_ToggleButton>();
	bool SetFocus = FocusButton->GetActive();

	Widget_List* List = Widget->mItemButtonList.Get<Widget_List>();
	int FocusIndex = List->GetElementIndex(Source);

	// Make sure only one Item is active at the same time
	if (SetFocus)
	{
		auto DeactiveNotFocusLamdba = [&](FUnitHandle& Handle)
		{
			if (Handle != Source)
			{
				Widget_ToggleButton* Button = Handle.Get<Widget_ToggleButton>();
				Button->SetActive(false);
			}
		};

		List->ForEachItem(DeactiveNotFocusLamdba);
	}

	// Tell the inventory to focus on the new item/container
	FUnitHandle Handle = Widget->mContainer->GetContentsByIndex(FocusIndex);
	if (Handle.IsValid())
	{
		ActionManager::Get().SetActionFocus(Handle);
	}
	else
	{
		ActionManager::Get().ClearActionFocus();
	}
}
/****************************************************************************************/

