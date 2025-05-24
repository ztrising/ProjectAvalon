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
class AvalonActor;

/***************************************************************************************
*  Widget_Inventory
*
*  Manages all item container widgets
*  When a pouch is selected, opens an item panel
* 
****************************************************************************************/
class Widget_Inventory : public AvalonWidget
{
public:
	Widget_Inventory();
	~Widget_Inventory();

private:
	HardRefList mItemPouches;
	HardRefList mItemPouchContainers;

	void RequestContainerOpen(IContainer* Container);
	void RequestContainerClosed(IContainer* Container);

	void OpenLevelContainer(IContainer* Container);
	void CloseLevelContainer();
	void HideLevelContainer();

	// Containers you can open in the environment
	HardUnitRef mLevelContainer;
	HardUnitRef mCloseButton;

	// TODO, should be HardUnitRef... once I get to the animation systems
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
	static void HandleItemAdded(IAvalonUnit* Listener, AvalonActor* AddedItem);
	static void HandleItemRemoved(IAvalonUnit* Listener, AvalonActor* RemovedItem);

	static void HandleContainerOpened(IAvalonUnit* Listener, IContainer* Container);
	static void HandleContainerClosed(IAvalonUnit* Listener, IContainer* Container);

	static void OnPlayerStartTravel(IAvalonUnit* Listener, Traveller* Traveller);

	static void OnCloseAnimFinished(IAvalonUnit* Listener, IAvalonAnimation* Animation);
	
	static void HandleButtonPressed(IAvalonUnit* Listener, const Widget_Button* Source);
	/****************************************************************************************/
};


