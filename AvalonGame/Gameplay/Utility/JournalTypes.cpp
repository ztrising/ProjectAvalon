/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "JournalTypes.h"

FJournalEntry* FJournalEntry::mCurrentEntry = nullptr;

void FJournalEntry::Load(FSaveContext& Context)
{
	mTimeStamp.Load("Time", Context);

	// TODO:
	// We should cache the year for display, but store it as zero on load
	mTimeStamp.mTimeStamp[ETimeScale::ETS_Years].mValue = 0;

	mPos.Load("Pos", Context);

	Context.LoadTextString(mText);

	//strip it of end lines and tabs
	mText.erase(remove(mText.begin(), mText.end(), '\t'), mText.end());
	mText.erase(remove(mText.begin(), mText.end(), '\n'), mText.end());
}

void FJournalEntry::Save(FSaveContext& Context)
{
	mTimeStamp.Save("Time", Context);
	mPos.Save("Pos", Context);

	Context.SaveTextString(mText);
}

/*static*/ void FJournalEntry::ReloadCurrentEntry()
{
	ISaveable::LoadJournalEntry(*FJournalEntry::mCurrentEntry, 0);
}

/*static*/ FJournalEntry* FJournalEntry::GetCurrentEntry()
{
	if (FJournalEntry::mCurrentEntry == nullptr)
	{
		FJournalEntry::mCurrentEntry = new FJournalEntry;
		ReloadCurrentEntry();
	}

	return FJournalEntry::mCurrentEntry;
}

/*static*/ void FJournalEntry::GenerateEntry(FJournalEntry& OutEntry, std::string EntryText)
{

}

void FJournalEntry::AppendEntryText(std::string Text)
{
	// TODO: Implement a delimiter, so appended entries
	// can have white space between them!
	// 
	//mText = mText + '\n' + '\n' + Text;

	mText = mText + Text;
}

/***************************************************************************************
*  Journal Effects
****************************************************************************************/
void Effect_JournalEntry::ExecuteEffect(FEffectContext& Context)
{
	if (mNewEntry)
	{
		// Save the current entry to file
		FJournalEntry* CurrentEntry = FJournalEntry::GetCurrentEntry();
		ISaveable::SaveJournalEntry(*CurrentEntry);

		// Fill a new entry with the current location/time stamp
		FJournalEntry NewEntry;
		FJournalEntry::GenerateEntry(NewEntry, mEntryText);
		ISaveable::MakeNewJournalEntry(NewEntry);

		// Reload the current entry to point at new entry
		FJournalEntry::ReloadCurrentEntry();
	}
	else
	{
		FJournalEntry* CurrentEntry = FJournalEntry::GetCurrentEntry();
		CurrentEntry->AppendEntryText(mEntryText);
	}
}

void Effect_JournalEntry::Load(FSaveContext& Context)
{
	// Expected Layout:
	/*
		<JournalEntry NewEntry="true">
			Journal Entry Text goes here!
		</JournalEntry>
	*/

	Context.LoadTextString(mEntryText);
	Context.Load("NewEntry", mNewEntry);
}

void Effect_JournalEntry::Save(FSaveContext& Context)
{
	Context.SaveTextString(mEntryText);
	Context.Save("NewEntry", mNewEntry);
}
