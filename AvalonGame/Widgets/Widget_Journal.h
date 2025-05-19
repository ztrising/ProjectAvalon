/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "Widget_ToggleButton.h"

struct FJournalEntry;

class Widget_Journal : public Widget_ToggleButton
{
public:
	Widget_Journal();

private:
	FUnitHandle mJournalPage;
	void PopulateJournalPage();

	void PopulateEntry(const FJournalEntry& Entry, int EntryNumber);

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	void OnGameLoaded() override;
	/****************************************************************************************/

	/***************************************************************************************
	*  Widget_ToggleButton
	****************************************************************************************/
public:
	void SetActive(bool Active) override;
	/****************************************************************************************/
};
