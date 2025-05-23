/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "Widget_Button.h"

#include <string>

struct FEquipSlot;

class Widget_EquipSlot : public AvalonWidget, public IEventListener
{
public:
	~Widget_EquipSlot();
	void InitializeWithData(FEquipSlot* Data);

private:
	void PopulateItems();

	FEquipSlot* mSlotData = nullptr;
	HardUnitRef mEquipmentList;

	/***************************************************************************************
	*  AvalonWidget
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	/****************************************************************************************/

	/***************************************************************************************
	*  IEventListener
	****************************************************************************************/
public:
	static void HandleButtonPressed(IEventListener* Listener, const Widget_Button* Source);
	/****************************************************************************************/
};
