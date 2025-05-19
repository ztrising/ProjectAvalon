/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_EquipSlotButton.h"
#include "Widget_Equipment.h"
#include "../Gameplay/Actor/AvalonActor.h"
#include "../Gameplay/Components/ContainerComponent.h"
#include "../Gameplay/Actions/ActionManager.h"

void Widget_EquipSlotButton::SetSlottedActor(FUnitHandle Handle)
{
	mSlottedActor = Handle;

	FTextSettings TextSettings;
	TextSettings.mOffset = FCoord(0, 0);
	TextSettings.mExtent = FCoord(0, 1);
	TextSettings.mUseBufferExtentWidth = true;
	TextSettings.mJustified = ETextJustification::Center;
	TextSettings.mFillExtent = true;

	std::string DisplayName = "<< Empty >>";
	if (mSlottedActor.IsValid())
	{
		if (AvalonActor* EquipmentActor = mSlottedActor.Get<AvalonActor>())
		{
			TextSettings.mJustified = ETextJustification::Left;
			DisplayName = EquipmentActor->mDisplayName;

		}
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
		if (Active)
		{
			if (mSlottedActor.IsValid())
			{
				ActionManager::Get().SetActionFocus(mSlottedActor);

				AvalonActor* SlottedActor = mSlottedActor.Get<AvalonActor>();
				Equipment* EquipComp = SlottedActor->GetComponent<Equipment>();
				if (EquipComp)
				{
					mSubMenu = AddChild<Widget_Equipment>();
					Widget_Equipment* EquipWidget = mSubMenu.Get<Widget_Equipment>();
					EquipWidget->SetEquipmentRef(EquipComp);
					EquipWidget->SetPosition(FCoord(-27, 0));
				}
			}
		}
		else
		{
			if (mSlottedActor.IsValid())
			{
				ActionManager::Get().ClearActionFocus();
				RemoveChild<Widget_Equipment>(mSubMenu);
			}
		}
	}
}
/****************************************************************************************/
