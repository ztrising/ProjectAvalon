/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/
/***************************************************************************************
*  Widget_Equipment
*  
*  A widget that displays the equipment of the provided Equipment (container class)
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "Widget_Button.h"

#include "../Gameplay/Utility/EventDispatcher.h"

class Equipment;
class AvalonActor;

class Widget_Equipment : public AvalonWidget, public IEventListener
{
public:
	~Widget_Equipment();

	void SetEquipmentRef(Equipment* EquipmentContainer);

private:
	void PopulateEquipment();

	HardUnitRef mEquipmentContainerRef;
	HardUnitRef mSlotListRef;

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
	static void HandleEquipmentChanged(IEventListener* Listener, AvalonActor* Item);
	static void HandleButtonPressed(IEventListener* Listener, const Widget_Button* Source);
	/****************************************************************************************/
};
