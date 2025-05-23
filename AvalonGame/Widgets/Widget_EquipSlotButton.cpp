/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_EquipSlotButton.h"
#include "Widget_Equipment.h"
#include "../Gameplay/Actor/AvalonActor.h"
#include "../Gameplay/Components/ContainerComponent.h"
#include "../Gameplay/Actions/ActionManager.h"

void Widget_EquipSlotButton::SetSlottedActor(HardUnitRef ActorRef)
{
	mSlottedActor = ActorRef;

	FTextSettings TextSettings;
	TextSettings.mOffset = FCoord(0, 0);
	TextSettings.mExtent = FCoord(0, 1);
	TextSettings.mUseBufferExtentWidth = true;
	TextSettings.mJustified = ETextJustification::Center;
	TextSettings.mFillExtent = true;

	std::string DisplayName = "<< Empty >>";
	if (ActorRef != nullptr)
	{
		TextSettings.mJustified = ETextJustification::Left;
		DisplayName = Get<AvalonActor>(ActorRef)->mDisplayName;

	}

	SetTextWithSettings(DisplayName.c_str(), TextSettings);
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_EquipSlotButton::Construct(const char* WidgetAsset)
{
	Widget_ToggleButton::Construct(WidgetAsset);
}
/****************************************************************************************/

/***************************************************************************************
*  Widget_ToggleButton
****************************************************************************************/
void Widget_EquipSlotButton::SetActive(bool Active)
{
	bool ActiveValueChanged = (Active != mIsActive);

	Widget_ToggleButton::SetActive(Active);

	if (ActiveValueChanged)
	{
		HardUnitRef SlottedActorRef = mSlottedActor.lock();
		AvalonActor* SlottedActor = Get<AvalonActor>(SlottedActorRef);
		if (Active)
		{
			if (SlottedActor != nullptr)
			{
				ActionManager::Get().SetActionFocus(SlottedActorRef);
				Equipment* EquipComp = SlottedActor->GetComponent<Equipment>();
				if (EquipComp)
				{
					mSubMenu = AddChild<Widget_Equipment>();
					Widget_Equipment* EquipWidget = Get<Widget_Equipment>(mSubMenu);
					EquipWidget->SetEquipmentRef(EquipComp->GetSelfRef());
					EquipWidget->SetPosition(FCoord(-27, 0));
				}
			}
		}
		else
		{
			if (SlottedActor != nullptr)
			{
				ActionManager::Get().ClearActionFocus();
				RemoveChild<Widget_Equipment>(mSubMenu);
			}
		}
	}
}
/****************************************************************************************/
