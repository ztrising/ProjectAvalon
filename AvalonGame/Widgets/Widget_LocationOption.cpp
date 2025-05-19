/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_LocationOption.h"
#include "../Gameplay/Actor/AvalonActor.h"
#include "../Gameplay/Actions/ActionManager.h"

Widget_LocationOption::Widget_LocationOption()
{
	mTextSettings.mOffset = FCoord();
	mTextSettings.mExtent = FCoord();
	mTextSettings.mUseBufferExtentWidth = true;
	mTextSettings.mUseBufferExtentHeight = true;
	mTextSettings.mFillExtent = true;
	mTextSettings.mJustified = ETextJustification::Left;
}

void Widget_LocationOption::SetActorHandle(FUnitHandle Handle)
{
	mActorHandle = Handle;

	if (AvalonActor* Actor = mActorHandle.Get<AvalonActor>())
	{
		mText.Get<AvalonWidget>()->SetTextWithSettings(Actor->mDescription.c_str(), mTextSettings);
	}
	else
	{
		mText.Get<AvalonWidget>()->SetTextWithSettings("Invalid Actor Handle", mTextSettings);
	}
}

void Widget_LocationOption::SetLocationText(const char* Text)
{
	mText.Get<AvalonWidget>()->SetTextWithSettings(Text, mTextSettings);
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_LocationOption::Construct(const char* WidgetAsset)
{
	Widget_ToggleButton::Construct("W_LocPanel_Option.xml");
	mText = AddChild<AvalonWidget>("W_LocPanel_OptionText.xml");
	mSymbolPrompt = AddChild<AvalonWidget>("W_LocPanel_OptionPrompt.xml");
}

void Widget_LocationOption::OnFocusGained()
{
	Widget_ToggleButton::OnFocusGained();

	mSymbolPrompt.Get<AvalonWidget>()->ApplyAttributes(AVALON_FG_CYAN);
	mText.Get<AvalonWidget>()->SetPosition(FCoord(6,0));
}
void Widget_LocationOption::OnFocusLost()
{
	Widget_ToggleButton::OnFocusGained();

	mSymbolPrompt.Get<AvalonWidget>()->ApplyAttributes(AVALON_FG_WHITE);
	mText.Get<AvalonWidget>()->SetPosition(FCoord(5,0));
}
/****************************************************************************************/

/***************************************************************************************
*  Widget_ToggleButton
****************************************************************************************/
void Widget_LocationOption::SetActive(bool Active)
{
	Widget_ToggleButton::SetActive(Active);
	// Play animation

	ActionManager::Get().SetActionFocus(mActorHandle);
}
/****************************************************************************************/
