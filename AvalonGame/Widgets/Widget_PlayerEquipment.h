/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_ToggleButton.h"

/***************************************************************************************
*  Widget_PlayerEquipment
*
*  How we display the player equipment panel!
*  A toggle button to show and hide it!
* 
****************************************************************************************/
class Widget_PlayerEquipment : public Widget_ToggleButton
{
private:
	HardUnitRef mEquipmentPanelRef;

	void ShowEquipmentPanel();
	void HideEquipmentPanel();

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	void OnGameLoaded() override;
	/****************************************************************************************/

	/***************************************************************************************
	*  Widget_ToggleButton
	****************************************************************************************/
public:
	void SetActive(bool Active) override;
	/****************************************************************************************/
};


