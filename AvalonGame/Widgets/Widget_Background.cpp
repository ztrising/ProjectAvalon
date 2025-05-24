/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_Background.h"

#include "../Gameplay/GameTime/GameCalendar.h"

#include <stdlib.h>     /* srand, rand */
//#include <cmath>  // pow

Widget_Background::Widget_Background()
{
	mStartVisible = true;

	SetAvalonWidgetDebugName("Widget_Background");
}

void Widget_Background::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct("TitleScreen.xml");
	//AvalonWidget::Construct("W_HUD.xml");
}

void Widget_Background::OnGameLoaded()
{
	/* TODO: Dynamic Sky Stuff, Amy fix it later*/
	/*GameCalendar::GameTimeEvent::Callback Callback = Widget_Background::HandleTimeAdvanced;
	GameCalendar::Get()->BindEvent_TimeAdvanced(this, Callback);
	UpdateSkyColor();*/
	// TODO: Teardown unlisten

	AvalonWidget::OnGameLoaded();
}

/*static*/ void Widget_Background::HandleTimeAdvanced( IAvalonUnit* Listener
													 , long DeltaHours)
{
	Widget_Background* Widget = static_cast<Widget_Background*>(Listener);
	Widget->UpdateSkyColor();
}

/*
void Widget_Background::HandleFinishTravel(const FLocationInfo& NewLocation)
{
	FBufferAnimSettings Animation;// = FBufferAnimSettings(NewLocation.mFileName);
	Animation.mStyle = EAnimationStyle::FadeTo;
	Animation.mDirection = EAnimationDirection::TopToBottom;
	Animation.mLength = 1.25f;

	//Figure out how to load visuals from const files and game
	// data from the same load data function...
	//PlayAnimation(Animation);
}
*/

void Widget_Background::UpdateSkyColor()
{
	ApplyAttributes(0); // To force a redraw

	return;

	// SO! It would be cool to have a layer of the background
	//  that cycles between like orange for during the day
	//  And blue/purple at night

	// Bonus if we layered on "Weather" on top of that!  Like
	// rain and clouds.  But lets start with SKY COLOR.

	const FGameTime& GameTime = GameCalendar::GetCurrentTime();
	int Hour = GameTime.mTimeStamp[ETimeScale::ETS_Hours].mValue;

	int ScreenWidth = this->mBuffer.GetSize().X; // For testing!
	int ScreenHeight = (this->mBuffer.GetSize().Y / 3) * 2;
	float ScreenQuarter = float(ScreenWidth) / 4.f;
	float ScreenEighth = ScreenQuarter / 2.f;

	int TimeOfDayWidth = ScreenWidth / 2;

	/*
	unsigned short DawningColors[5]		= { AVALON_FG_BLACK,	AVALON_FG_BLUE,		AVALON_FG_RED,		AVALON_FG_WHITE,	AVALON_FG_YELLOW};
	unsigned short ForeNoonColors[5]	= { AVALON_FG_BLUE,		AVALON_FG_BLUE,		AVALON_FG_WHITE,	AVALON_FG_YELLOW,	AVALON_FG_WHITE };
	unsigned short NoonTideColors[5]	= { AVALON_FG_WHITE,	AVALON_FG_YELLOW ,	AVALON_FG_BLUE ,	AVALON_FG_WHITE ,	AVALON_FG_WHITE };
	unsigned short EvenTideColors[5]	= { AVALON_FG_WHITE,	AVALON_FG_YELLOW,	AVALON_FG_RED,		AVALON_FG_BLUE,		AVALON_FG_RED };
	unsigned short GloamingColors[5]	= { AVALON_FG_RED,		AVALON_FG_BLUE,		AVALON_FG_MAGENTA,	AVALON_FG_MAGENTA,	AVALON_FG_BLACK };
	unsigned short FullDarkColors[5]	= { AVALON_FG_MAGENTA,	AVALON_FG_BLACK,	AVALON_FG_BLACK,	AVALON_FG_BLACK,	AVALON_FG_BLACK };
	*/
	
	/*
	unsigned short DawningColors[5]		= {AVALON_FG_YELLOW,	AVALON_FG_YELLOW,	AVALON_FG_YELLOW,		AVALON_FG_YELLOW,		AVALON_FG_YELLOW};
	unsigned short ForeNoonColors[5]	= { AVALON_FG_BLUE,		AVALON_FG_BLUE,		AVALON_FG_BLUE,	AVALON_FG_BLUE,	AVALON_FG_BLUE };
	unsigned short NoonTideColors[5]	= { AVALON_FG_WHITE,	AVALON_FG_WHITE ,	AVALON_FG_WHITE ,	AVALON_FG_WHITE ,	AVALON_FG_WHITE };
	unsigned short EvenTideColors[5]	= { AVALON_FG_RED,	AVALON_FG_RED,	AVALON_FG_RED,		AVALON_FG_RED,		AVALON_FG_RED };
	unsigned short GloamingColors[5]	= { AVALON_FG_MAGENTA,		AVALON_FG_MAGENTA,		AVALON_FG_MAGENTA,	AVALON_FG_MAGENTA,		AVALON_FG_MAGENTA };
	unsigned short FullDarkColors[5]	= { AVALON_FG_BLACK,		AVALON_FG_BLACK,	AVALON_FG_BLACK,		AVALON_FG_BLACK,	AVALON_FG_BLACK };
	*/

	
	unsigned short DawningColors[5]		= { AVALON_FG_RED,		AVALON_FG_YELLOW,	AVALON_FG_YELLOW,	AVALON_FG_YELLOW,		AVALON_FG_YELLOW };
	unsigned short ForeNoonColors[5]	= { AVALON_FG_WHITE,	AVALON_FG_BLUE,	AVALON_FG_BLUE,		AVALON_FG_GREEN,		AVALON_FG_GREEN };
	unsigned short NoonTideColors[5]	= { AVALON_FG_BLUE,		AVALON_FG_YELLOW,	AVALON_FG_BLUE,		AVALON_FG_GREEN,		AVALON_FG_GREEN };
	unsigned short EvenTideColors[5]	= { AVALON_FG_RED,		AVALON_FG_MAGENTA,		AVALON_FG_RED,		AVALON_FG_GREEN,		AVALON_FG_GREEN };
	unsigned short GloamingColors[5]	= { AVALON_FG_RED,		AVALON_FG_BLACK,	AVALON_FG_MAGENTA,	AVALON_FG_GREEN,		AVALON_FG_GREEN };
	unsigned short FullDarkColors[5]	= { AVALON_FG_MAGENTA,	AVALON_FG_WHITE,	AVALON_FG_BLACK,	AVALON_FG_GREEN,		AVALON_FG_GREEN };


	//unsigned short BG_Colors[6] = { AVALON_BG_MAGENTA,	AVALON_BG_YELLOW,	AVALON_BG_WHITE,	AVALON_BG_YELLOW, AVALON_BG_MAGENTA,  AVALON_BG_BLACK };
	int HourSection = 0;

	unsigned short FG_Colors[24] = {	AVALON_FG_BLACK,	AVALON_FG_BLACK,	AVALON_FG_BLACK,	AVALON_FG_BLACK,
										AVALON_FG_YELLOW,	AVALON_FG_YELLOW,	AVALON_FG_RED,		AVALON_FG_YELLOW,
										AVALON_FG_WHITE,	AVALON_FG_YELLOW,		AVALON_FG_WHITE,	AVALON_FG_WHITE,
										AVALON_FG_WHITE,	AVALON_FG_WHITE,	AVALON_FG_WHITE,		AVALON_FG_YELLOW,
										AVALON_FG_RED,		AVALON_FG_RED,		AVALON_FG_BLUE,		AVALON_FG_RED,
										AVALON_FG_MAGENTA,	AVALON_FG_MAGENTA,	AVALON_FG_MAGENTA,	AVALON_FG_MAGENTA };

	unsigned short BG_Colors[24] = {	AVALON_BG_BLACK,	AVALON_BG_BLACK,	AVALON_BG_BLACK,	AVALON_BG_BLACK,
										AVALON_BG_BLACK,	AVALON_BG_YELLOW,	AVALON_BG_WHITE,	AVALON_BG_YELLOW,
										AVALON_BG_WHITE,	AVALON_BG_YELLOW,	AVALON_BG_WHITE,	AVALON_BG_WHITE,
										AVALON_BG_WHITE,	AVALON_BG_WHITE,	AVALON_BG_YELLOW,	AVALON_BG_YELLOW,
										AVALON_BG_RED,		AVALON_BG_YELLOW,	AVALON_BG_WHITE,	AVALON_BG_RED,
										AVALON_BG_MAGENTA,	AVALON_BG_BLACK,	AVALON_BG_BLACK,	AVALON_BG_BLACK };

	ETimeOfDay TimeOfDay;
	float TimeProgress;
	GameTimeUtils::GetTimeOfDay(GameTime, TimeOfDay, TimeProgress);

	int ToDInt = TimeOfDay - 1;
	if (ToDInt < 0)
	{
		ToDInt = ETimeOfDay::ETOD_Fulldark;
	}

	ETimeOfDay RecentPast = ETimeOfDay(ToDInt);

	ToDInt = TimeOfDay + 1;
	if (ToDInt >= ETimeOfDay::ETOD_Count)
	{
		ToDInt = ETimeOfDay::ETOD_Dawning;
	}

	ETimeOfDay NearFuture = ETimeOfDay(ToDInt);

	unsigned short TestChars[9] = { '.', '.', '.', '.', '.', '.', '.', '.', '.' };
	auto BG_CharPicker_Lambda = [&TestChars](int X, int Y)
	{
		//if (TimeOfDay == ETimeOfDay::)
		unsigned short RetValue = TestChars[((X * Y) % 9)];;

		return RetValue;
	};
	/*
	auto BG_ColorPicker_Lambda = [&](int X, int Y, unsigned short& OutChar)
	{
		unsigned short RetValue = 0;

		int Chance = rand() % 100 + 1;


		int ToD = 1;

		ETimeOfDay Default_ToD = TimeOfDay;
		ETimeOfDay Adjust_ToD = RecentPast;

		int ColorSection = TimeOfDay;
		int ColorIndex = 1;
		int AdjustIndex = 2;
		float ScreenPct = float(X) / float(ScreenWidth);
		float ChanceToAdjust = 100.f;
		float SwapThreshold = 1.f - TimeProgress;

		if (ScreenPct < SwapThreshold)   // Chance to show Recent Past colors
		{
			if (ScreenPct < .25f)
			{
				ChanceToAdjust *= .5f;
			}

			float AjustmentSectionPct = float(X) / (SwapThreshold * ScreenWidth);

			// Make displaying recent past less likely the further right we are
			ChanceToAdjust *= (1.f - AjustmentSectionPct);
		}
		else // Chance to show Near Future colors
		{
			if (ScreenPct > .75f)
			{
				ChanceToAdjust *= .5f;
			}

			float AjustmentSectionPct = (float(X) - (SwapThreshold * ScreenWidth)) / (ScreenWidth - (SwapThreshold * ScreenWidth));

			Adjust_ToD = NearFuture;
			AdjustIndex = 0;

			// Make displaying the near future more likely the further right we are
			ChanceToAdjust *= AjustmentSectionPct;
		}

		if (ChanceToAdjust > Chance)
		{
			ColorIndex = AdjustIndex;
			ColorSection = Adjust_ToD;
		}

		if (Chance > 50)
		{
			// half the time we just draw BG color
			OutChar = ' ';
			return BG_Colors[ColorSection];
		}

		unsigned short* ColorArray = nullptr;
		switch (ColorSection)
		{
		case 0: ColorArray = DawningColors;	 OutChar = '#'; break;
		case 1: ColorArray = ForeNoonColors; OutChar = '#'; break;
		case 2: ColorArray = NoonTideColors; OutChar = '#'; break;
		case 3: ColorArray = EvenTideColors; OutChar = '#'; break;
		case 4: ColorArray = GloamingColors; OutChar = '#'; break;
		case 5: 
		default: ColorArray = FullDarkColors; OutChar = '*'; break;
		}

		//int ColorIndex = int(IndexScalar * 5);

		//int Deviation = rand() % 2;
		int ColorChance = rand() % 100 + 1;
		if (ColorChance > 25)
		{
			ColorIndex = 0;
		}
		else 
		{
			ColorIndex = 1;
		}

		RetValue = BG_Colors[ColorSection] | ColorArray[ColorIndex];

		return RetValue;
	};
	*/


	auto BG_NewPicker_Lambda = [&](int X, unsigned short& OutChar)
	{
		float ScreenPct = float(X) / float(ScreenWidth);
		int LeftHour = (Hour - 2);
		if (LeftHour < 0)
		{
			LeftHour += 24;
		}

		//int RightHour = (Hour + 4) % 24;

		int XHour = (LeftHour + int(ScreenPct * 5)) % 24;

		OutChar = '#';

		int FG_Devi = rand() % 4;
		int FG_Index = (XHour - 2 + FG_Devi) % 24;

		int BG_Devi = rand() % 4;
		int BG_Index = (XHour - 2 + BG_Devi) % 24;

		return BG_Colors[BG_Index] | FG_Colors[FG_Index];
	};

	for (int PosX = 0; PosX < ScreenWidth; ++PosX)
	{

		for (int PosY = 0; PosY < ScreenHeight; ++PosY)
		{
			FCoord ScreenPos = FCoord(PosX, PosY);

			unsigned short Char;// = BG_CharPicker_Lambda(PosX, PosY);
			unsigned short Att = BG_NewPicker_Lambda(PosX, Char);

			
			mBuffer.SetCellData(ScreenPos, Char, Att);
		}
	}
}