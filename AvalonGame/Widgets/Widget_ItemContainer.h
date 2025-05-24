/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "Widget_Button.h"
#include "../Gameplay/Actor/ActorTypes.h"

#include "../Gameplay/Utility/EventDispatcher.h"

class ItemContainer;

class Widget_ItemContainer : public AvalonWidget
{
public:
	~Widget_ItemContainer();

	void SetContainer(ItemContainer* NewContainer);
	void PopulateItems();
	void DeselectAll();
	ItemContainer* GetContainer() { return Get<ItemContainer>(mItemContainerRef); }

private:
	HardUnitRef mItemContainerRef;
	HardUnitRef mItemButtonListRef;
	
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
	static void HandleContentsChanged(IAvalonUnit* Listener, AvalonActor* Item);
	static void HandleButtonPressed(IAvalonUnit* Listener, const Widget_Button* Source);
	/****************************************************************************************/
};
