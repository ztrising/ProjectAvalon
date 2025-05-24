/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "../Gameplay/Utility/EventDispatcher.h"

/***************************************************************************************
*  Widget_Background
*
*  Draws the Frame and Background for the current location
****************************************************************************************/

class Widget_Background : public AvalonWidget
{
public:
	Widget_Background();

private:
	void UpdateSkyColor();

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
	static void HandleTimeAdvanced(IAvalonUnit* Listener, long DeltaHours);
	// TODO: Listen for Travel
	/****************************************************************************************/
};
