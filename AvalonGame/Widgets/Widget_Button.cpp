/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_Button.h"
#include "../../AvalonUtilities/AvalonTypes.h"

#include "../../AvalonGame/AvalonGame.h"

Widget_Button::Widget_Button()
{
	SetAvalonWidgetDebugName("Widget_Button");

	mStartVisible  = true;
	mWantsFocus    = true;
	mDefaultColor  = (AVALON_FG_WHITE);						// White Foreground, Black background
	mFocussedColor = (AVALON_FG_BLUE | AVALON_FG_INTENSITY);// Intense Blue Foreground, Black background, AVALON_FG_BLUE
}

void Widget_Button::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct(WidgetAsset);

	ApplyAttributes(mDefaultColor);
}

void Widget_Button::OnFocusGained()
{
	AvalonWidget::OnFocusGained();
	ApplyAttributes(GetFocussedColor());
}

void Widget_Button::OnFocusLost()
{
	AvalonWidget::OnFocusLost();
	ApplyAttributes(GetNotFocussedColor());
}

unsigned short Widget_Button::GetFocussedColor()
{
	return mFocussedColor;
}

unsigned short Widget_Button::GetNotFocussedColor()
{
	return mDefaultColor;
}

void Widget_Button::OnButtonPressed()
{

}

bool Widget_Button::HandleInput(const FInputKeyEventParams& EventParams)
{
	if (mHasFocus && EventParams.mKeyDown)
	{
		if (   EventParams.mEvent == EInputEvent::EConfirm
			|| EventParams.mEvent == EInputEvent::ELeftMouseButton)
		{
			OnButtonPressed();
			mOnButtonPressed.BroadcastEvent(GetSelfHandle());
			return true;
		}
	}

	return false;
}
