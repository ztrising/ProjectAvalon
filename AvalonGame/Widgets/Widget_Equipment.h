/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "Widget_Button.h"

#include "../Gameplay/Utility/EventDispatcher.h"

class Equipment;

class Widget_Equipment : public AvalonWidget, public IEventListener
{
public:
	~Widget_Equipment();

	void SetEquipmentRef(Equipment* EquipmentRef);

private:
	void PopulateEquipment();

	Equipment* mEquipment = nullptr;
	FUnitHandle mSlotList;

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
	static void HandleEquipmentChanged(IEventListener* Listener, FUnitHandle& Item);
	static void HandleButtonPressed(IEventListener* Listener, const FUnitHandle& Source);
	/****************************************************************************************/
};
