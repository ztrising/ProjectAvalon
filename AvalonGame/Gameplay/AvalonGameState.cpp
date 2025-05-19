/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonGameState.h"
#include "../AssetLoaders/AssetLoader.h" // Saving/Loading, maybe move to ISaveable

#include "Utility/JournalTypes.h"
#include "GameTime/GameCalendar.h"
#include "Actor/LevelActor.h"
#include "Actor/PlayerActor.h"
#include "../Widgets/AvalonHUD.h"

void AvalonGameState::Init()
{
	//LoadGame();
}

void AvalonGameState::TearDown()
{

}

void AvalonGameState::NewGame()
{
	AssetLoaders::CreateNewSave();
	LoadGame();
}

void AvalonGameState::LoadGame()
{
	mIsLoaded = true;

	FJournalEntry* Entry = FJournalEntry::GetCurrentEntry();

	/***************************************************************************************
	*  Load The Calendar
	****************************************************************************************/
	GameCalendar::LoadCalendar(Entry->GetTimeStamp());
	/****************************************************************************************/

	/***************************************************************************************
	*  Load The World
	****************************************************************************************/
	FLocationInfo::mWorldMap = new FLocationInfo();
	FLocationInfo::mWorldMap->SetConstData();
	ISaveable::LoadFromFile(FLocationInfo::mWorldMap, "WorldAtlas.xml");

	const FWorldPos& Pos = Entry->GetWorldPos();
	FLocationInfo* Location = MapUtils::GetLocation(Pos, EGeoScale::EGS_Location);
	LevelActor::OpenLevel(LevelActor::mCurrentLevel, Location);
	
	FLocationInfo* Area = MapUtils::GetLocation(Pos, EGeoScale::EGS_Area);
	LevelActor::OpenLevel(LevelActor::mStreamingLevel, Area);
	/****************************************************************************************/

	/***************************************************************************************
	*  Load The Player IN the World
	****************************************************************************************/
	PlayerActor::LoadPlayer();
	/****************************************************************************************/

	AvalonHUD::GetHUD().HandleGameLoaded();
}

void AvalonGameState::SaveGame()
{
	// TODO:  Write Game Time to Journal

	// TODO:  Save The World (and location to Journal)
}

void AvalonGameState::TickGame(float DeltaSeconds)
{
	if (mIsLoaded)
	{
		GameCalendar* TheGameCalendar = GameCalendar::Get();
		if (TheGameCalendar != nullptr)
		{
			TheGameCalendar->Tick(DeltaSeconds);
		}

		if (LevelActor::mCurrentLevel != nullptr)
		{
			LevelActor::mCurrentLevel->Tick(DeltaSeconds);
		}
		
		if (LevelActor::mStreamingLevel != nullptr)
		{
			LevelActor::mStreamingLevel->Tick(DeltaSeconds);
		}
	}
}
