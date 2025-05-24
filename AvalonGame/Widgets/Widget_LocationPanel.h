/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "Widget_Button.h"
#include "../Gameplay/Utility/EventDispatcher.h"

#include <vector>

struct FLocationInfo;
class IContainer;
class AvalonActor;

class Widget_LocationPanel : public AvalonWidget
{
public:
	Widget_LocationPanel();
	~Widget_LocationPanel();

private:
	void PopulateFromLocation();
	void PopulateTravelling();

	//void PopulateWorldPosInfo(const FLocationInfo* LocationInfo);  // Where did this go?

	//FUnitHandle mTitleText;
	HardUnitRef mDescTextRef;
	HardRefList mLocationActors;

	// Let's try the "Floor" and "Dialogue" and "Shops" and other things
	// being panels that pop to the right of the Location
	// "Left/Right" Panel Test
	void MoveLeft();
	void MoveRight();
	void StartInspectingFloor(IContainer* Container);
	void StopInspectingFloor(IContainer* Container);

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	void OnGameLoaded() override;
	void Show() override;
	/****************************************************************************************/

	/***************************************************************************************
	*  IEventListener
	****************************************************************************************/
public:
	static void HandleLevelActorsChanged(IAvalonUnit* Listener, AvalonActor* Actor);
	static void OnPlayerFinishTravel(IAvalonUnit* Listener, class Traveller* PlayerTraveller);
	static void HandleContainerOpened(IAvalonUnit* Listener, IContainer* Container);
	static void HandleContainerClosed(IAvalonUnit* Listener, IContainer* Container);
	/****************************************************************************************/
};


