/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonGameState.h"
#include "../AssetLoaders/AssetLoader.h" // Saving/Loading, maybe move to ISaveable

#include "Utility/JournalTypes.h"
#include "GameTime/GameCalendar.h"

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
	OpenCurrentLevelAt(Location);
	
	FLocationInfo* Area = MapUtils::GetLocation(Pos, EGeoScale::EGS_Area);
	OpenStreamingLevelAt(Area);
	/****************************************************************************************/

	/***************************************************************************************
	*  Load The Player IN the World
	****************************************************************************************/
	LoadPlayer();
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

		if (mCurrentLevel != nullptr)
		{
			AvalonActor::Get<LevelActor>(mCurrentLevel)->Tick(DeltaSeconds);
		}
		
		if (mStreamingLevel != nullptr)
		{
			AvalonActor::Get<LevelActor>(mStreamingLevel)->Tick(DeltaSeconds);
		}
	}
}

/*static*/ AvalonActor* AvalonGameState::GetPlayerActor()
{
	AvalonGameState& GameState = GetGameState();
	return AvalonActor::Get<AvalonActor>(GameState.mPlayerRef);
}

/*static*/ LevelActor* AvalonGameState::GetCurrentLevel()
{
	AvalonGameState& GameState = GetGameState();
	return AvalonActor::Get<LevelActor>(GameState.mCurrentLevel);
}

/*static*/ void AvalonGameState::OpenCurrentLevelAt(FLocationInfo* Location)
{
	AvalonGameState& GameState = GetGameState();
	GameState.OpenLevel_Internal(GameState.mCurrentLevel, Location);
}

/*static*/ void AvalonGameState::OpenStreamingLevelAt(FLocationInfo* Location)
{
	AvalonGameState& GameState = GetGameState();
	GameState.OpenLevel_Internal(GameState.mStreamingLevel, Location);
}

void AvalonGameState::OpenLevel_Internal(HardUnitRef& LevelRef, FLocationInfo* Location)
{
	LevelActor* Level = AvalonActor::Get<LevelActor>(LevelRef);
	if (Level != nullptr)
	{
		if (Level->GetLocation() == Location)
		{
			// This location is already opened!
			return;
		}

		Level->OnLevelClosed();		// Unbinds events, and saves the level file
		LevelRef.reset();			// Let go of the reference, should cause it to be cleaned up
	}

	AvalonMemory::NewUnit<LevelActor>(LevelRef);
	Level = AvalonActor::Get<LevelActor>(LevelRef);
	Level->SetLocation(Location);
	Level->OnLevelOpened();
}

/*static*/ void AvalonGameState::SetActorTravelling(HardUnitRef& Actor)
{
	AvalonGameState& GameState = GetGameState();
	LevelActor* StreamingLevel = AvalonActor::Get<LevelActor>(GameState.mStreamingLevel);

	LevelActor::MoveActorTo(Actor, StreamingLevel);
}

/*static*/ void AvalonGameState::FinishActorTravel(HardUnitRef& Actor)
{

}

/*static*/ void AvalonGameState::LoadPlayer()
{
	AvalonGameState& GameState = GetGameState();

	AvalonMemory::NewUnit<PlayerActor>(GameState.mPlayerRef);
	LevelActor::PlaceActorInLevel(GameState.mPlayerRef, AvalonActor::Get<LevelActor>(GameState.mCurrentLevel));

	AvalonActor* PlayerActor = AvalonActor::Get<AvalonActor>(GameState.mPlayerRef);
	ISaveable::LoadFromFile(PlayerActor, "PlayerActor.xml");
}

/*static*/ void AvalonGameState::SavePlayer()
{
	AvalonGameState& GameState = GetGameState();

	AvalonActor* PlayerActor = AvalonActor::Get<AvalonActor>(GameState.mPlayerRef);
	ISaveable::SaveToFile(PlayerActor, "PlayerActor.xml");
}