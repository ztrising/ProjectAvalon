/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonActor.h"

struct FLocationInfo;
class IContainer;

/***************************************************************************************
*  Level Actor - Areas, Regions, Locations
****************************************************************************************/
class LevelActor : public AvalonActor
{
public:
    LevelActor();
    ~LevelActor();

    void Tick(float DeltaSeconds) override;
    void AdvanceTime(long DeltaHours) override;

    void OnLevelOpened();
    void OnLevelClosed();

    // For level transitions
    static void OpenLevel(LevelActor*& Level, struct FLocationInfo* NewLocation);

    // For actors NOT in levels (Players Spawning)
    static void PlaceActorInLevel(FUnitHandle& ActorHandle, LevelActor* NewLevel);

    // For actors in a level moving to another level (Player/Travellers)
    static void MoveActorTo(FUnitHandle& ActorHandle, LevelActor* NewLevel);

    static LevelActor* mCurrentLevel;
    static LevelActor* mStreamingLevel;

    // Gets a list of Actors "placed" in the level, doesn't include actors "within" those actors.
    void GetActorsPlacedInLevel(std::vector<FUnitHandle>& OutActors) const;

    void SetLevelContainer(class LevelContainer* Container) { mLevelContainer = Container; }
    IContainer* GetFloorContainer() { return mFloorContainer; }

private:
    void InitializeDefaultComponents();

    class LevelContainer* mLevelContainer = nullptr;
    IContainer* mFloorContainer = nullptr;

    /***************************************************************************************
    *  IEventListener
    ****************************************************************************************/
public:
    static void HandleTimeAdvanced(IEventListener* Listener, long HoursAdvanced);
    /****************************************************************************************/

    /***************************************************************************************
    *  Actor Memory Management
    ****************************************************************************************/
public:
    FUnitHandle LoadActor(const FUnitHandle& Owner, FSaveContext& Context);
    /****************************************************************************************/
};
