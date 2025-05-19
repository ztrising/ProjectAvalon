/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_Journal.h"
#include "../Gameplay/Utility/JournalTypes.h"
#include "../AssetLoaders/AssetLoader.h"

Widget_Journal::Widget_Journal()
{
	mValidHUDContexts = EHUDContext::LOCATION | EHUDContext::TRAVELLING;
	mStartVisible = true;
}

void Widget_Journal::PopulateJournalPage()
{
	int EntryNumber = 0;
	int MaxEntriesShown = 3;

	FJournalEntry Entry;
	while ( (EntryNumber < MaxEntriesShown) && 
			ISaveable::LoadJournalEntry(Entry, EntryNumber))
	{
		PopulateEntry(Entry, EntryNumber);
		EntryNumber++;
	}

	FBufferAnimSettings Animation = FBufferAnimSettings();
	Animation.mStyle		= EAnimationStyle::FadeIn;
	Animation.mDirection	= EAnimationDirection::DrizzleDown;
	Animation.mLength = 7.f;
	//mJournalPage.Get()->PlayAnimation(Animation);
}

void Widget_Journal::PopulateEntry(const FJournalEntry& Entry, int EntryNumber)
{
	FCoord EntryPosition;
	EntryPosition.X = 20;
	EntryPosition.Y = 8 + (10 * EntryNumber);

	FTextSettings TextSettings;

	TextSettings.mUseBufferExtentWidth = false;
	TextSettings.mUseBufferExtentHeight = false;
	TextSettings.mJustified = ETextJustification::Left;
	TextSettings.mFillExtent = true;
	TextSettings.mFillChar = ' ';

	// Draw Game Time
	TextSettings.mOffset = EntryPosition + FCoord(0, 0);
	TextSettings.mExtent = FCoord(60, 1);
	//char GameTimeString[100];
	//Entry.mTimeStamp.PopulateString(GameTimeString);

	AvalonWidget* PageWidget = mJournalPage.Get<AvalonWidget>();

	const char* GameTimeString = "24th of Beltane, Yr 1132";
	PageWidget->SetTextWithSettings(GameTimeString, TextSettings);

	// Draw Location
	TextSettings.mOffset = EntryPosition + FCoord(0, 1);
	TextSettings.mExtent = FCoord(60, 1);

	const FWorldPos& Pos = Entry.GetWorldPos();
	std::string LocName = Pos[EGeoScale::EGS_Location];
	PageWidget->SetTextWithSettings(LocName.c_str(), TextSettings);

	// Draw Text
	std::string EntryText = Entry.GetText();
	TextSettings.mOffset = EntryPosition + FCoord(0, 3);
	TextSettings.mExtent = FCoord(60, 7);
	PageWidget->SetTextWithSettings(EntryText.c_str(), TextSettings);
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_Journal::Construct(const char* WidgetAsset)
{
	Widget_ToggleButton::Construct("W_Journal_Button.xml");
	mJournalPage = AddChild<AvalonWidget>("W_Journal_Page.xml");
	mJournalPage.Get<AvalonWidget>()->Hide();
}

void Widget_Journal::OnGameLoaded()
{
	AvalonWidget::OnGameLoaded();
	SetActive(true);
}
/****************************************************************************************/

/***************************************************************************************
*  Widget_ToggleButton
****************************************************************************************/
void Widget_Journal::SetActive(bool Active)
{
	Widget_ToggleButton::SetActive(Active);

	AvalonWidget* PageWidget = mJournalPage.Get<AvalonWidget>();

	if (Active)
	{
		PopulateJournalPage();
		PageWidget->Show();
	}
	else
	{
		PageWidget->Hide();
	}
}
/****************************************************************************************/
