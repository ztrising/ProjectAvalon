/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_ToggleButton.h"

Widget_ToggleButton::Widget_ToggleButton()
{
	mActiveColor = (AVALON_FG_CYAN | AVALON_FG_INTENSITY);
	mActiveFocussedColor = mActiveColor;
}

Widget_ToggleButton::~Widget_ToggleButton()
{
	int i = 0;
}

void Widget_ToggleButton::Construct(const char* WidgetAsset)
{
	Widget_Button::Construct(WidgetAsset);
}

void Widget_ToggleButton::SetActive(bool Active)
{
	if (mIsActive != Active)
	{
		mIsActive = Active;
		ApplyAttributes(mHasFocus ? GetFocussedColor() : GetNotFocussedColor());
	}
}

void Widget_ToggleButton::OnButtonPressed()
{
	SetActive(!mIsActive);
}

unsigned short Widget_ToggleButton::GetFocussedColor()
{
	return mIsActive ? mActiveFocussedColor : Widget_Button::GetFocussedColor();
}

unsigned short Widget_ToggleButton::GetNotFocussedColor()
{
	return mIsActive ? mActiveColor : Widget_Button::GetNotFocussedColor();
}

