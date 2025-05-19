/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_EquipSlot.h"
#include "Widget_List.h"
#include "Widget_EquipSlotButton.h"
#include "Widget_Equipment.h"

#include "../Gameplay/Components/ContainerComponent.h"
#include "../Gameplay/Actor/AvalonActor.h"


Widget_EquipSlot::~Widget_EquipSlot()
{
	Widget_List* List = mEquipmentList.Get<Widget_List>();
	List->EmptyList<Widget_EquipSlotButton>();

	RemoveChild<Widget_List>(mEquipmentList);
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_EquipSlot::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct(WidgetAsset);
	mEquipmentList = AddChild<Widget_List>();
	mEquipmentList.Get<AvalonWidget>()->SetPosition(FCoord(2, 2));
}
/****************************************************************************************/

void Widget_EquipSlot::InitializeWithData(FEquipSlot* Data)
{
	mSlotData = Data;

	FTextSettings TextSettings;
	TextSettings.mOffset = FCoord(7, 1);
	TextSettings.mExtent = FCoord(16, 1);
	TextSettings.mUseBufferExtentWidth = false;
	TextSettings.mJustified = ETextJustification::Right;
	TextSettings.mFillExtent = true;
	TextSettings.mFillChar = ' ';

	SetTextWithSettings(mSlotData->mID.c_str(), TextSettings);

	PopulateItems();
}

void Widget_EquipSlot::PopulateItems()
{
	if (mSlotData == nullptr)
	{
		return;
	}

	int NumEntries = mSlotData->mMaxCapacity;

	/***************************************************************************************
	*  Equipment in Slot
	****************************************************************************************/
	Widget_List* List = mEquipmentList.Get<Widget_List>();

	FWidgetListSettings ListSettings;
	ListSettings.mMaxHorizontalElements = 1;
	ListSettings.mMaxVerticalElements = 5;

	List->ConstructList<Widget_EquipSlotButton>(NumEntries, ListSettings, "W_EquipmentButton.xml");
	auto It = mSlotData->mSlotContents.begin();

	// The Equipment Page that is showing this slot, listens for the buttons
	// to be pressed!
	AvalonWidget* ListParent = GetParent().Get<AvalonWidget>();
	Widget_Equipment* ParentListener = ListParent->GetParent().Get<Widget_Equipment>();

	auto InitSlotEntryLambda = [&](FUnitHandle& Handle)
	{
		FUnitHandle EquipmentHandle;
		if (It != mSlotData->mSlotContents.end())
		{
			EquipmentHandle = (*It);
			It++;
		}

		Widget_EquipSlotButton* SlotButton = Handle.Get<Widget_EquipSlotButton>();
		SlotButton->SetSlottedActor(EquipmentHandle);

		Widget_Button::ButtonEvent::Callback ButtonPressedCallback = Widget_Equipment::HandleButtonPressed;
		SlotButton->mOnButtonPressed.BindEvent(ParentListener, ButtonPressedCallback);
	};

	List->ForEachItem(InitSlotEntryLambda);
	/****************************************************************************************/

	// For each OCCUPIED slot, shows a button with the NAME of the item

	// For each UNOCCUPIED slot, shows a button that says "empty slot"
		// If the button is clicked:
			// Another window pops up, with a list of ALL possible things that can
			// be equipped to that slot, (that aren't already equipped)

			// Clicking on an item in that list will automagically equip it to the
			// slot.

}

/***************************************************************************************
*  Button Event Handler
****************************************************************************************/
/*static*/ void Widget_EquipSlot::HandleButtonPressed( IEventListener* Listener
													 , const FUnitHandle& Source)
{
	Widget_EquipSlot* Widget = static_cast<Widget_EquipSlot*>(Listener);

	auto ExclusiveEquipActiveLamba = [&](FUnitHandle& Handle)
	{
		if (Source != Handle)
		{
			Widget_EquipSlotButton* Button = Handle.Get<Widget_EquipSlotButton>();
			Button->SetActive(false);
		}
	};

	Widget_List* List = Widget->mEquipmentList.Get<Widget_List>();
	List->ForEachItem(ExclusiveEquipActiveLamba);
}
/****************************************************************************************/
