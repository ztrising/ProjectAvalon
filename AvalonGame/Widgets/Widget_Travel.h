/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "../Gameplay/Utility/EventDispatcher.h"

class Traveller;
struct FLocationInfo;

class Widget_Travel : public AvalonWidget
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
	static void OnStartTravel(IAvalonUnit* Listener, Traveller* InTraveller);
	static void OnFinishTravel(IAvalonUnit* Listener, Traveller* InTraveller);
	/****************************************************************************************/

private:
	void PopulateTravelText();
};
