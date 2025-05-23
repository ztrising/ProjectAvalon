/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "../Gameplay/Utility/EventDispatcher.h"

class Widget_GameTime : public AvalonWidget, public IEventListener
{
public:
	Widget_GameTime();

private:
	HardUnitRef mTextAreaRef;

	void PopulateTimeText();
	void PopulateTravellingText();

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset);
	void OnGameLoaded() override;
	/****************************************************************************************/

	/***************************************************************************************
	*  IEventListener
	****************************************************************************************/
public:
	static void HandleTimeAdvanced(IEventListener* Listener, long HoursAdvanced);
	/****************************************************************************************/
};
