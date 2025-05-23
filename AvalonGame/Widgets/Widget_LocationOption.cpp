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

void Widget_LocationOption::SetLocationActorRef(HardUnitRef ActorRef)
{
	mLocationActorRef = ActorRef;

	AvalonActor* Actor = Get<AvalonActor>(mLocationActorRef);
	if (Actor != nullptr)
	{
		Get<AvalonWidget>(mTextWidgetRef)->SetTextWithSettings(Actor->mDescription.c_str(), mTextSettings);
	}
	else
	{
		Get<AvalonWidget>(mTextWidgetRef)->SetTextWithSettings("Invalid Actor Handle", mTextSettings);
	}
}

void Widget_LocationOption::SetLocationText(const char* Text)
{
	Get<AvalonWidget>(mTextWidgetRef)->SetTextWithSettings(Text, mTextSettings);
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_LocationOption::Construct(const char* WidgetAsset)
{
	Widget_ToggleButton::Construct("W_LocPanel_Option.xml");
	mTextWidgetRef = AddChild<AvalonWidget>("W_LocPanel_OptionText.xml");
	mSymbolPromptRef = AddChild<AvalonWidget>("W_LocPanel_OptionPrompt.xml");
}

void Widget_LocationOption::OnFocusGained()
{
	Widget_ToggleButton::OnFocusGained();

	Get<AvalonWidget>(mSymbolPromptRef)->ApplyAttributes(AVALON_FG_CYAN);
	Get<AvalonWidget>(mTextWidgetRef)->SetPosition(FCoord(6,0));
}
void Widget_LocationOption::OnFocusLost()
{
	Widget_ToggleButton::OnFocusGained();

	Get<AvalonWidget>(mSymbolPromptRef)->ApplyAttributes(AVALON_FG_WHITE);
	Get<AvalonWidget>(mTextWidgetRef)->SetPosition(FCoord(5,0));
}
/****************************************************************************************/

/***************************************************************************************
*  Widget_ToggleButton
****************************************************************************************/
void Widget_LocationOption::SetActive(bool Active)
{
	Widget_ToggleButton::SetActive(Active);
	// Play animation

	ActionManager::Get().SetActionFocus(mLocationActorRef.lock());
}
/****************************************************************************************/
