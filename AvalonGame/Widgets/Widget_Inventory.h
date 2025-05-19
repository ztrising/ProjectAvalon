/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_Button.h"
#include "../Gameplay/Utility/EventDispatcher.h"


#include <vector>

struct FActorHandle;
class IContainer;
class Traveller;

/***************************************************************************************
*  Widget_Inventory
*
*  Manages all item container widgets
*  When a pouch is selected, opens an item panel
* 
****************************************************************************************/
class Widget_Inventory : public AvalonWidget, public IEventListener
{
public:
	Widget_Inventory();
	~Widget_Inventory();

private:
	std::vector<FUnitHandle> mItemPouches;
	std::vector<FUnitHandle> mItemPouchContainers;

	void RequestContainerOpen(IContainer* Container);
	void RequestContainerClosed(IContainer* Container);

	void OpenLevelContainer(IContainer* Container);
	void CloseLevelContainer();
	void HideLevelContainer();

	// Containers you can open in the environment
	FUnitHandle mLevelContainer;
	FUnitHandle mCloseButton;
	IAvalonAnimation* mCloseAnimation = nullptr;

	// TODO: Special Trading Panel...
	//FWidgetHandle mTradeInventory;

	// Need to create an event that tells us when to do this!
	void PopulatePouches();

	/***************************************************************************************
	*  AvalonWidget
	****************************************************************************************/
public:
	void Show() override;
	void Hide() override;
	void Construct(const char* WidgetAsset) override;
	void OnGameLoaded() override;
	/****************************************************************************************/

	/***************************************************************************************
	*  IEventListener
	****************************************************************************************/
public:
	static void HandleItemAdded(IEventListener* Listener, FUnitHandle& AddedItem);
	static void HandleItemRemoved(IEventListener* Listener, FUnitHandle& RemovedItem);

	static void HandleContainerOpened(IEventListener* Listener, IContainer* Container);
	static void HandleContainerClosed(IEventListener* Listener, IContainer* Container);

	static void OnPlayerStartTravel(IEventListener* Listener, Traveller* Traveller);

	static void OnCloseAnimFinished(IEventListener* Listener, IAvalonAnimation* Animation);
	
	static void HandleButtonPressed(IEventListener* Listener, const FUnitHandle& Source);
	/****************************************************************************************/
};


