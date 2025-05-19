/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"

class Widget_PlayerPanel : public AvalonWidget
{
public:
	Widget_PlayerPanel();

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	void OnGameLoaded() override;
	/****************************************************************************************/
};
