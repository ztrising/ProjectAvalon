/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"

class Widget_Button : public AvalonWidget
{
public:
	Widget_Button();

protected:
	virtual unsigned short GetFocussedColor();
	virtual unsigned short GetNotFocussedColor();
	virtual void OnButtonPressed();

	unsigned short mDefaultColor = 0;
	unsigned short mFocussedColor = 0;

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	virtual void Construct(const char* WidgetAsset) override;
	virtual void OnFocusGained() override;
	virtual void OnFocusLost() override;
	/****************************************************************************************/

	/***************************************************************************************
	*  Avalon Input Listener Interface
	****************************************************************************************/
public:
	bool HandleInput(const FInputKeyEventParams& EventParams) override;
	/****************************************************************************************/

	/***************************************************************************************
	*   Button Event!
	****************************************************************************************/
public:
	typedef FEventDispatcher<const Widget_Button*> ButtonEvent;
	ButtonEvent mOnButtonPressed;
	/****************************************************************************************/
};
