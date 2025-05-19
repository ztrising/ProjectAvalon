/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "Widget_Button.h"

#include "../Gameplay/Utility/EventDispatcher.h"

class ItemContainer;

class Widget_ItemContainer : public AvalonWidget, public IEventListener
{
public:
	~Widget_ItemContainer();

	void SetContainer(ItemContainer* NewContainer);
	void PopulateItems();
	void DeselectAll();
	ItemContainer* GetContainer() { return mContainer; }

private:
	ItemContainer* mContainer = nullptr;
	FUnitHandle mItemButtonList;
	
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
	static void HandleContentsChanged(IEventListener* Listener, FUnitHandle& Item);
	static void HandleButtonPressed(IEventListener* Listener, const FUnitHandle& Source);
	/****************************************************************************************/
};
