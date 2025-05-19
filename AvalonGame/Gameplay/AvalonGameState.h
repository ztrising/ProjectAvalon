/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../AvalonUnit/AvalonUnit.h"

struct FJournalEntry;
class GameCalendar;

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
