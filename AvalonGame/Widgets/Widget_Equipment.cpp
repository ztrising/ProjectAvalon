/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_Equipment.h"
#include "Widget_List.h"
#include "Widget_EquipSlot.h"
#include "Widget_EquipSlotButton.h"

#include "../Gameplay/Components/ContainerComponent.h"
#include "../Gameplay/Actor/AvalonActor.h"

Widget_Equipment::~Widget_Equipment()
{
	SetEquipmentRef(nullptr);

	Widget_List* List = mSlotList.Get<Widget_List>();
	List->EmptyList<Widget_EquipSlot>();

	RemoveChild<Widget_List>(mSlotList);
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_Equipment::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct("W_EquipmentPanel.xml");
	mSlotList = AddChild<Widget_List>();
	mSlotList.Get<AvalonWidget>()->SetPosition(FCoord(0, 1));
}
/****************************************************************************************/

void Widget_Equipment::SetEquipmentRef(Equipment* EquipmentRef)
{
	if (mEquipment != nullptr)
	{
		// Unbind event
		AvalonActor* Actor = mEquipment->GetActorOwner();
		Actor->mOnItemAdded.UnBindEvent(this);
		Actor->mOnItemRemoved.UnBindEvent(this);
	}

	mEquipment = EquipmentRef;

	if (mEquipment != nullptr)
	{
		// Bind event
		AvalonActor* Actor = mEquipment->GetActorOwner();

		AvalonActor::InventoryEvent::Callback Callback = Widget_Equipment::HandleEquipmentChanged;
		Actor->mOnItemAdded.BindEvent(this, Callback);
		Actor->mOnItemRemoved.BindEvent(this, Callback);

		// Set name
		FUnitHandle Outer = EquipmentRef->GetActorOwnerHandle();
		std::string DisplayName = Outer.Get<AvalonActor>()->mDisplayName;

		SetText(DisplayName.c_str(), FCoord(0, 0));

		PopulateEquipment();
	}
}

void Widget_Equipment::PopulateEquipment()
{
	if (mEquipment == nullptr)
	{
		return;
	}

	int NumSlots = mEquipment->mEquipment.size();

	/***************************************************************************************
	*  Equipment Slots
	****************************************************************************************/
	Widget_List* List = mSlotList.Get<Widget_List>();

	FWidgetListSettings ListSettings;
	ListSettings.mMaxHorizontalElements = 1;
	ListSettings.mMaxVerticalElements = 10;
	ListSettings.mVerticalPadding = 0;

	List->ConstructList<Widget_EquipSlot>(NumSlots, ListSettings, "W_EquipSlot.xml");

	auto It = mEquipment->mEquipment.begin();
	auto InitSlotLambda = [&](FUnitHandle& Handle)
	{
		Widget_EquipSlot* Slot = Handle.Get<Widget_EquipSlot>();
		Slot->InitializeWithData(&((*It)));

		It++;
	};

	List->ForEachItem(InitSlotLambda);
	/****************************************************************************************/
}

/***************************************************************************************
*  IEventListener
****************************************************************************************/
/*static*/ void Widget_Equipment::HandleEquipmentChanged( IEventListener* Listener
													    , FUnitHandle& Item)
{
	Widget_Equipment* Widget = static_cast<Widget_Equipment*>(Listener);
	Widget->PopulateEquipment();
}

/*static*/ void Widget_Equipment::HandleButtonPressed( IEventListener* Listener
													 , const FUnitHandle& Source)
{
	Widget_Equipment* Widget = static_cast<Widget_Equipment*>(Listener);

	auto ExclusiveSlotActiveLamba = [&](FUnitHandle& Handle)
	{
		Widget_EquipSlot* Slot = Handle.Get<Widget_EquipSlot>();
		Slot->HandleButtonPressed(Slot, Source);
	};

	Widget_EquipSlotButton* SourceButton = Source.Get<Widget_EquipSlotButton>();
	if (SourceButton->GetActive())
	{
		Widget_List* List = Widget->mSlotList.Get<Widget_List>();
		List->ForEachItem(ExclusiveSlotActiveLamba);
	}
}
/****************************************************************************************/

