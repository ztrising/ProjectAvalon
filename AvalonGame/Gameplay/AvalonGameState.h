/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../AvalonUnit/AvalonUnit.h"
#include "Actor/ActorTypes.h"
#include "Actor/LevelActor.h"
#include "Actor/PlayerActor.h"

struct FJournalEntry;
class GameCalendar;
class LevelActor;

/***************************************************************************************
*  Avalon Game State - Gameplay Logic
****************************************************************************************/
class AvalonGameState
{
public:
    void Init();
    void TearDown();

    void TickGame(float DeltaSeconds);

    void NewGame();
    void SaveGame();
    void LoadGame();

private:
    bool mIsLoaded = false;

    //////////////////////////////////////////////////////////////////////////
    //  The loaded levels
public:
    static void OpenCurrentLevelAt(struct FLocationInfo* Location);
    static void OpenStreamingLevelAt(struct FLocationInfo* Location);

    static void SetActorTravelling(HardUnitRef& Actor);
    static void FinishActorTravel(HardUnitRef& Actor);

private:
    HardUnitRef mCurrentLevel = nullptr;
    HardUnitRef mStreamingLevel = nullptr;

    void OpenLevel_Internal(HardUnitRef& Level, FLocationInfo* Location);
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    //  The Player!
public:
    static AvalonActor* GetPlayerActor();
    static LevelActor* GetCurrentLevel();

    static void LoadPlayer();
    static void SavePlayer();

private:
    HardUnitRef mPlayerRef = nullptr;
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    //  Singleton Implementation
public:
    static AvalonGameState& GetGameState()
    {
        static AvalonGameState GameState;
        return GameState;
    }

    AvalonGameState(AvalonGameState const&) = delete;
    void operator=(AvalonGameState const&) = delete;

private:
    AvalonGameState() {};
    ~AvalonGameState() {};
    //////////////////////////////////////////////////////////////////////////
};
