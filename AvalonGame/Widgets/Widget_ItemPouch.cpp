/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_ItemPouch.h"
#include "Widget_Inventory.h"

Widget_ItemPouch::Widget_ItemPouch()
{
	mDefaultColor = 0;
	mFocussedColor = (AVALON_FG_YELLOW | AVALON_FG_INTENSITY);

	mActiveColor = 0;
	mActiveFocussedColor = (AVALON_FG_YELLOW | AVALON_FG_INTENSITY);

	mPouchOpenAnimation = new FBufferAnimSettings("W_ItemPouch_Open.xml");
	mPouchOpenAnimation->mStyle		= EAnimationStyle::FadeTo;
	mPouchOpenAnimation->mDirection = EAnimationDirection::DrizzleUp;
	mPouchOpenAnimation->mLength	= .25f;


	mPouchCloseAnimation = new FBufferAnimSettings("W_ItemPouch_Closed.xml");
	mPouchCloseAnimation->mStyle	 = EAnimationStyle::FadeTo;
	mPouchCloseAnimation->mDirection = EAnimationDirection::DrizzleDown;
	mPouchCloseAnimation->mLength	 = .5f;
}

Widget_ItemPouch::~Widget_ItemPouch()
{
	delete mPouchOpenAnimation;
	delete mPouchCloseAnimation;
}

void Widget_ItemPouch::Construct(const char* WidgetAsset)
{
	Widget_ToggleButton::Construct("W_ItemPouch_Frame.xml");
	mPouchImage = AddChild<AvalonWidget>("W_ItemPouch_Closed.xml");
}

void Widget_ItemPouch::SetActive(bool Active)
{
	Widget_ToggleButton::SetActive(Active);

	if (mIsActive)
	{
		mPouchImage.Get<AvalonWidget>()->PlayAnimation(*mPouchOpenAnimation);
	}
	else
	{
		mPouchImage.Get<AvalonWidget>()->PlayAnimation(*mPouchCloseAnimation);
	}
}
