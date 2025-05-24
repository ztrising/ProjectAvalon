/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_GameTime.h"
#include "AvalonHUD.h"
#include "../Gameplay/GameTime/GameCalendar.h"

#include <string>

Widget_GameTime::Widget_GameTime()
{
	mStartVisible = false;
}

void Widget_GameTime::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct("W_GameTime_Frame.xml");
	mTextAreaRef = AddChild<AvalonWidget>("W_GameTime_TextArea.xml");
}

void Widget_GameTime::OnGameLoaded()
{
	AvalonWidget::OnGameLoaded();

	PopulateTimeText();
	PopulateTravellingText();

	GameCalendar::GameTimeEvent::Callback Callback = Widget_GameTime::HandleTimeAdvanced;
	GameCalendar::BindEvent_TimeAdvanced(this, Callback);
	// TODO: Teardown unlisten

	// Listen for Travel Begin
	// Listen for Travel End

	Show();
}

void Widget_GameTime::PopulateTimeText()
{
	AvalonWidget* TextWidget = Get<AvalonWidget>(mTextAreaRef);

	const FGameTime& CurrentTime = GameCalendar::GetCurrentTime();
	int Years	= CurrentTime.mTimeStamp[ETimeScale::ETS_Years].mValue + 1;
	int Months	= CurrentTime.mTimeStamp[ETimeScale::ETS_Months].mValue;
	int Days	= CurrentTime.mTimeStamp[ETimeScale::ETS_Days].mValue;
	int Hours	= CurrentTime.mTimeStamp[ETimeScale::ETS_Hours].mValue;

	// How many years the player has been playing
	std::string TimeString = "     Year " + std::to_string(Years);

	FCoord TextPos = FCoord(4, 0);
	TextWidget->SetText(TimeString.c_str(), TextPos, 0, false, true);

	TimeString = GameTimeUtils::GetDayString(Days);
	TimeString = TimeString + " of " + GameTimeUtils::GetMonthName(Months) + "," + std::to_string(Years + 1022);

	TextPos = FCoord(4, 1);
	TextWidget->SetText(TimeString.c_str(), TextPos);

	int DisplayHour = (Hours);
	if (DisplayHour > 12)
	{
		DisplayHour -= 12;
	}
	else if (DisplayHour == 0)
	{
		DisplayHour = 12;
	}

	std::string Suffix = (Hours > 11) ? " pm" : " am";
	TimeString = std::to_string(DisplayHour) + Suffix;

	TextPos = FCoord(7, 4);
	TextWidget->SetText(TimeString.c_str(), TextPos);

	TimeString = GameTimeUtils::GetTimeOfDayString(CurrentTime);

	TextPos = FCoord(6, 3);
	TextWidget->SetText(TimeString.c_str(), TextPos);
}

void Widget_GameTime::PopulateTravellingText()
{
	AvalonWidget* TextWidget = Get<AvalonWidget>(mTextAreaRef);
	FCoord TravellingTextPos = FCoord(4, 6);


	bool IsTravelling = false;//AvalonListeners::IsTravelling();
	if (IsTravelling)
	{
		TextWidget->SetText("Traveling...", TravellingTextPos, 0);
	}
}

/***************************************************************************************
*  GameCalendar::OnTimeAdvanced Callback
****************************************************************************************/
/*static*/ void Widget_GameTime::HandleTimeAdvanced(IAvalonUnit* Listener, long HoursAdvanced)
{
	Widget_GameTime* Widget = static_cast<Widget_GameTime*>(Listener);
	Widget->PopulateTimeText();
}
/****************************************************************************************/