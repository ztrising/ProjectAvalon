/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_ToggleButton.h"
#include "../Gameplay/Actor/ActorTypes.h"

class Widget_EquipSlotButton : public Widget_ToggleButton
{
public:
	void SetSlottedActor(FUnitHandle Handle);

private:
	FUnitHandle mSlottedActor;
	FUnitHandle mSubMenu;

	/***************************************************************************************
	*  Widget_ToggleButton
	****************************************************************************************/
public:
	void SetActive(bool Active) override;
	/****************************************************************************************/

	/***************************************************************************************
	*  AvalonWidget
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	/****************************************************************************************/
};
