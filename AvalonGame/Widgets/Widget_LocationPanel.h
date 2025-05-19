/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "Widget_Button.h"
#include "../Gameplay/Utility/EventDispatcher.h"

#include <vector>

struct FLocationInfo;
struct FActorHandle;
class IContainer;

class Widget_LocationPanel : public AvalonWidget, public IEventListener
{
public:
	Widget_LocationPanel();
	~Widget_LocationPanel();

private:
	void PopulateFromLocation();
	void PopulateTravelling();

	void PopulateWorldPosInfo(const FLocationInfo* LocationInfo);

	//FUnitHandle mTitleText;
	FUnitHandle mDescText;

	std::vector<FUnitHandle> mLocationActors;

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
	static void HandleLevelActorsChanged(IEventListener* Listener, FUnitHandle& Handle);
	static void OnPlayerFinishTravel(IEventListener* Listener, class Traveller* PlayerTraveller);
	static void HandleContainerOpened(IEventListener* Listener, IContainer* Container);
	static void HandleContainerClosed(IEventListener* Listener, IContainer* Container);
	/****************************************************************************************/
};


