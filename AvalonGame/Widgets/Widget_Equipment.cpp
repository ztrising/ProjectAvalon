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

	Widget_List* List = Get<Widget_List>(mSlotListRef);
	if (List)
	{
		List->EmptyList<Widget_EquipSlot>();
	}

	RemoveChild<Widget_List>(mSlotListRef);
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_Equipment::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct("W_EquipmentPanel.xml");
	mSlotListRef = AddChild<Widget_List>();

	Widget_List* List = Get<Widget_List>(mSlotListRef);
	List->SetPosition(FCoord(0, 1));
}
/****************************************************************************************/

void Widget_Equipment::SetEquipmentRef(Equipment* EquipmentContainer)
{
	if (mEquipmentContainerRef != nullptr)
	{
		Equipment* OldEquipmentContainer = Get<Equipment>(mEquipmentContainerRef);

		// Unbind event
		AvalonActor* Actor = OldEquipmentContainer->GetActorOwner();
		Actor->mOnItemAdded.UnBindEvent(this);
		Actor->mOnItemRemoved.UnBindEvent(this);
	}

	if (EquipmentContainer != nullptr)
	{
		mEquipmentContainerRef = EquipmentContainer->GetSelfRef();
	}

	if (mEquipmentContainerRef != nullptr)
	{
		Equipment* NewEquipmentContainer = Get<Equipment>(mEquipmentContainerRef);

		// Bind event
		AvalonActor* Actor = NewEquipmentContainer->GetActorOwner();

		AvalonActor::InventoryEvent::Callback Callback = Widget_Equipment::HandleEquipmentChanged;
		Actor->mOnItemAdded.BindEvent(this, Callback);
		Actor->mOnItemRemoved.BindEvent(this, Callback);

		// Set name
		//HardUnitRef Outer = EquipmentRef->GetActorOwner();
		std::string DisplayName = Actor->mDisplayName;

		SetText(DisplayName.c_str(), FCoord(0, 0));

		PopulateEquipment();
	}
}

void Widget_Equipment::PopulateEquipment()
{
	if (mEquipmentContainerRef == nullptr)
	{
		return;
	}

	Equipment* EquipmentContainer = Get<Equipment>(mEquipmentContainerRef);
	int NumSlots = EquipmentContainer->mEquipment.size();

	/***************************************************************************************
	*  Equipment Slots
	****************************************************************************************/
	Widget_List* List = Get<Widget_List>(mSlotListRef);

	FWidgetListSettings ListSettings;
	ListSettings.mMaxHorizontalElements = 1;
	ListSettings.mMaxVerticalElements = 10;
	ListSettings.mVerticalPadding = 0;

	List->ConstructList<Widget_EquipSlot>(NumSlots, ListSettings, "W_EquipSlot.xml");

	auto It = EquipmentContainer->mEquipment.begin();
	auto InitSlotLambda = [&](HardUnitRef& WidgetRef)
	{
		Widget_EquipSlot* Slot = Get<Widget_EquipSlot>(WidgetRef);
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
													    , AvalonActor* Item)
{
	Widget_Equipment* Widget = static_cast<Widget_Equipment*>(Listener);
	Widget->PopulateEquipment();
}

/*static*/ void Widget_Equipment::HandleButtonPressed( IEventListener* Listener
													 , const Widget_Button* Source)
{
	Widget_Equipment* Widget = static_cast<Widget_Equipment*>(Listener);

	auto ExclusiveSlotActiveLamba = [&](HardUnitRef& WidgetRef)
	{
		Widget_EquipSlot* Slot = AvalonWidget::Get<Widget_EquipSlot>(WidgetRef);
		Slot->HandleButtonPressed(Slot, Source);
	};

	const Widget_EquipSlotButton* SourceButton = static_cast<const Widget_EquipSlotButton*>(Source);
	if (SourceButton->GetActive())
	{
		Widget_List* List = Get<Widget_List>(Widget->mSlotListRef);
		List->ForEachItem(ExclusiveSlotActiveLamba);
	}
}
/****************************************************************************************/

