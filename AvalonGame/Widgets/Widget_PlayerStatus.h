/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"

#include "../Gameplay/Utility/EventDispatcher.h"

#include <map>
#include <string>

class Widget_PlayerStatus : public AvalonWidget, public IEventListener
{
public:
	Widget_PlayerStatus();

private:
	std::map<std::string, HardUnitRef> mStatDisplays;

	/***************************************************************************************
	*  IEventListener
	****************************************************************************************/
public:
	static void HandleStatValueChange(IEventListener* Listener, std::string StatID);
	void HandleStatValueChange(std::string Stat, float Value, float MaxValue);
	/****************************************************************************************/

	/***************************************************************************************
	*  AvalonWidget
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	void Show() override;
	void OnGameLoaded() override;
	/****************************************************************************************/
};

