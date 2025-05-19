/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Saveable.h"
#include "../GameTime/GameTimeTypes.h"
#include "../Map/TravelTypes.h"
#include "../Actions/AvalonAction.h"

struct FJournalEntry : public ISaveable
{
public:
	static void ReloadCurrentEntry();
	static FJournalEntry* GetCurrentEntry();
	static void GenerateEntry(FJournalEntry& OutEntry, std::string EntryText);

private:
	static FJournalEntry* mCurrentEntry;

public:
	const FGameTime& GetTimeStamp() const { return mTimeStamp; }
	const FWorldPos& GetWorldPos() const { return mPos; }
	std::string GetText() const { return mText; }

	void AppendEntryText(std::string Text);

private:
	FGameTime mTimeStamp;
	FWorldPos mPos;
	std::string mText;

private:
	/***************************************************************************************
	*  ISaveable 
	****************************************************************************************/
	void Load(FSaveContext& Context) override;
	void Save(FSaveContext& Context) override;
	/****************************************************************************************/
};

/***************************************************************************************
*  Journal Effects
****************************************************************************************/
struct Effect_JournalEntry : public IAvalonEffect
{
	void Load(FSaveContext& Context) override;
	void Save(FSaveContext& Context) override;

	void ExecuteEffect(FEffectContext& Context) override;

	std::string mEntryText;
	bool mNewEntry;
};
