/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "../Gameplay/Utility/EventDispatcher.h"

class Traveller;
struct FLocationInfo;

class Widget_Travel : public AvalonWidget, public IEventListener
{
public:
	Widget_Travel();
	~Widget_Travel();

	/***************************************************************************************
	*  AvalonWidget
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	void OnGameLoaded() override;
	/****************************************************************************************/

	/***************************************************************************************
	*  IEventListener
	****************************************************************************************/
public:
	static void OnStartTravel(IEventListener* Listener, Traveller* InTraveller);
	static void OnFinishTravel(IEventListener* Listener, Traveller* InTraveller);
	/****************************************************************************************/

private:
	void PopulateTravelText();
};
