/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_Button.h"

class Widget_ToggleButton : public Widget_Button
{
public:
	Widget_ToggleButton();
	~Widget_ToggleButton();

	virtual void SetActive(bool Active);
	bool GetActive() const { return mIsActive; }

	/***************************************************************************************
	*  Widget_Button Interface
	****************************************************************************************/
protected:
	unsigned short GetFocussedColor() override;
	unsigned short GetNotFocussedColor() override;
	virtual void OnButtonPressed() override;
	/****************************************************************************************/

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	/****************************************************************************************/

protected:
	bool mIsActive = false;

	unsigned short mActiveColor = 0;
	unsigned short mActiveFocussedColor = 0;
};
