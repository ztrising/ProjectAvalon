/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AssetLoader.h"
#include "../../ExternalTools/TinyXML/tinyxml2.h"

// blerg strlen
#include <stdio.h>
#include <string>

// TODO: MAKE THIS PLATFORM AGNOSTIC
//#include <Windows.h>  //wchar_t
#include <filesystem>


// Short Path:							"Loc_KaerMorhen_Greathall.xml"
// Relative Path:						"Locations/Loc_KaerMorhen_Greathall.xml"
// Full 'Installed Content' Path:		"AvalonContent/Locations/Loc_KaerMorhen_Greathall.xml"
// Full 'Save Data' Path:				"AvalonSaves/SaveName/Locations/Loc_KaerMorhen_Greathall.xml"

namespace AssetLoaders
{
	std::filesystem::path LoadedGamePath;
	const char* SaveSlotName = "SaveSlot1";
	const char* SaveFileName = "AvalonSave.xml";
}

/***************************************************************************************
*  Save/Load
****************************************************************************************/
std::filesystem::path GetContentPath()
{
	std::filesystem::path RunningPath = std::filesystem::current_path();
	std::filesystem::path ContentPath = RunningPath / "AvalonContent";

	return ContentPath;
}

std::filesystem::path GetSaveGamesPath()
{
	std::filesystem::path RunningPath = std::filesystem::current_path();
	std::filesystem::path SavesPath	  = RunningPath / "AvalonSaveData";

	return SavesPath;
}

std::filesystem::path GetCurrentSaveFile()
{
	std::filesystem::path CurrentSave = GetSaveGamesPath() / AssetLoaders::SaveSlotName / AssetLoaders::SaveFileName;

	return CurrentSave;
}

// Attempts to open the Current Loaded Saves version of the File
	// if that fails, it creates it in the current save directory
void OpenAvalonFileWithPath(std::filesystem::path RelativePath, tinyxml2::XMLDocument& OutFile)
{
	std::filesystem::path FileToLoad = AssetLoaders::LoadedGamePath / RelativePath;

	// No such file!  Let's create it
	if (!std::filesystem::exists(FileToLoad))
	{
		// Our files on one directory down...
		std::filesystem::path LoadCheck = FileToLoad;
		LoadCheck.remove_filename();
		std::filesystem::create_directory(LoadCheck);

		// Then copy the content template to the save data directory
		std::filesystem::path TemplateFile = GetContentPath() / RelativePath;
		std::filesystem::copy_file(TemplateFile, FileToLoad);
	}

	OutFile.LoadFile(FileToLoad.string().c_str());
}

void SaveAvalonFileWithPath(std::filesystem::path RelativePath, tinyxml2::XMLDocument& InFile)
{
	std::filesystem::path FileToSave = AssetLoaders::LoadedGamePath / RelativePath;

	// What if it doesn't exist?  Shouldn't it?

	InFile.SaveFile(FileToSave.string().c_str());
}

void AssetLoaders::LoadGame()
{
	LoadedGamePath = GetSaveGamesPath() / AssetLoaders::SaveSlotName;
	std::filesystem::path SaveFile = LoadedGamePath / AssetLoaders::SaveFileName;

	if (!std::filesystem::exists(SaveFile))
	{
		AssetLoaders::CreateNewSave();
	}
}

void AssetLoaders::CreateNewSave()
{
	std::filesystem::path AllSavesPath= GetSaveGamesPath();
	std::filesystem::create_directory(AllSavesPath);

	std::filesystem::path NewSavePath = AllSavesPath / AssetLoaders::SaveSlotName;
	std::filesystem::create_directory(NewSavePath);

	std::filesystem::path TemplateSaveFile = GetContentPath() / "GameData" / "Journal.xml";
	std::filesystem::path NewSaveFile = NewSavePath / AssetLoaders::SaveFileName;

	if (!std::filesystem::exists(NewSaveFile))
	{
		std::filesystem::copy_file(TemplateSaveFile, NewSaveFile);
	} 

	LoadedGamePath = NewSavePath;
}

void AssetLoaders::GetLoadedGameSave(char* OutSaveFile)
{
	std::filesystem::path SaveFile = AssetLoaders::LoadedGamePath / AssetLoaders::SaveFileName;
	strncpy_s(OutSaveFile, 200, SaveFile.string().c_str(), 200);
}

void AssetLoaders::OpenJournalFile(tinyxml2::XMLDocument& OutFile)
{
	OpenAvalonFileWithPath(GetCurrentSaveFile(), OutFile);
}

void AssetLoaders::SaveJournal(tinyxml2::XMLDocument& InFile)
{
	SaveAvalonFileWithPath(GetCurrentSaveFile(), InFile);
}

/*

THIS IS STILL REALLY USEUFL

*/
/*void AssetLoaders::LoadJournalEntry(FJournalEntry& OutJournalEntry, int EntryFromLast)
{
	tinyxml2::XMLDocument doc;
	OpenAvalonFileWithPath(GetCurrentSaveFile(), doc);

	tinyxml2::XMLElement* AvalonData = doc.FirstChildElement("AvalonData");
	tinyxml2::XMLElement* JournalRoot = AvalonData->FirstChildElement("Journal");

	tinyxml2::XMLElement* LastEntry = JournalRoot->LastChildElement("JournalEntry");
	
	tinyxml2::XMLElement* Entry = LastEntry;

	int EntryCount = EntryFromLast;
	while (Entry && EntryCount > 0)
	{
		Entry = Entry->PreviousSiblingElement();
		EntryCount--;
	}

	if (Entry == nullptr)
	{
		Entry = LastEntry;
	}

	tinyxml2::XMLElement* TimeElement = Entry->FirstChildElement("AvalonTime");

	// Journal Time Stamp
	//TimeElement->QueryIntAttribute("Year", &OutJournalEntry.mTimeStamp.mYears);
	//TimeElement->QueryIntAttribute("Month", &OutJournalEntry.mTimeStamp.mMonths);
	//TimeElement->QueryIntAttribute("Day", &OutJournalEntry.mTimeStamp.mDays);
	//TimeElement->QueryIntAttribute("Hour", &OutJournalEntry.mTimeStamp.mHours);

	// Journal Location
	tinyxml2::XMLElement* LocationElement = Entry->FirstChildElement("AvalonLocation");

	const char* FileName = "failed";
	LocationElement->QueryStringAttribute("LocFile", &FileName);
	//AssetLoaders::PopulateLocationFromFile(FileName, OutJournalEntry.mLocation);

	// Journal Text
	tinyxml2::XMLElement* TextElement = Entry->FirstChildElement("JournalText");

	const char* JournalText = "failure";
	TextElement->QueryStringAttribute("Text", &JournalText);
	//strncpy_s(OutJournalEntry.mText, AVALON_MAX_TEXT_XLRG, JournalText, AVALON_MAX_TEXT_XLRG);
}

void AssetLoaders::SaveJournalEntry(const FJournalEntry& InJournalEntry)
{
	// TODO
}*/
/****************************************************************************************/

/***************************************************************************************
*  Utility
****************************************************************************************/
void OpenConstFileWithPath(std::filesystem::path RelativePath, tinyxml2::XMLDocument& OutFile)
{
	std::filesystem::path FileToLoad = GetContentPath() / RelativePath;
	OutFile.LoadFile(FileToLoad.string().c_str());
}

void AssetLoaders::OpenConstFile(const char* Asset, tinyxml2::XMLDocument& OutFile)
{
	std::filesystem::path RelativePath = "GameData";
	RelativePath = RelativePath / Asset;

	std::filesystem::path Path = RelativePath;
	OpenConstFileWithPath(Path, OutFile);
}

void AssetLoaders::OpenWidgetFile(const char* WidgetAsset, tinyxml2::XMLDocument& OutFile)
{
	static const char* AVALON_WIDGET_DIR = "Widgets";
	std::filesystem::path RelativePath = AVALON_WIDGET_DIR;
	RelativePath = RelativePath / WidgetAsset;

	OpenConstFileWithPath(RelativePath, OutFile);
}

void AssetLoaders::OpenAvalonFile(const char* RelativePath, tinyxml2::XMLDocument& OutFile)
{
	std::filesystem::path Path = RelativePath;
	OpenAvalonFileWithPath(Path, OutFile);
}

void AssetLoaders::OpenGameDataFile(const char* Asset, tinyxml2::XMLDocument& OutFile)
{
	std::filesystem::path RelativePath = "GameData";
	RelativePath = RelativePath / Asset;

	OpenAvalonFileWithPath(RelativePath, OutFile);
}

void AssetLoaders::SaveGameDataFile(const char* Asset, tinyxml2::XMLDocument& InFile)
{
	std::filesystem::path RelativePath = "GameData";
	RelativePath = RelativePath / Asset;

	SaveAvalonFileWithPath(RelativePath, InFile);
}
/****************************************************************************************/

