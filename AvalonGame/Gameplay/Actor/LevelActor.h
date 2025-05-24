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

    LevelActor* GetLevel() override { return this; }

    // For actors NOT in levels (Players Spawning, Travellers Moving)
    static void PlaceActorInLevel(HardUnitRef& ActorRef, LevelActor* Level);
    static void RemoveActorFromLevel(HardUnitRef& ActorRef, LevelActor* Level);

    // For actors in a level moving to another level (Player/Travellers)
    static void MoveActorTo(HardUnitRef& ActorRef, LevelActor* Level);

    // Gets a list of Actors "placed" in the level, doesn't include actors "within" those actors.
    void GetActorsPlacedInLevel(HardRefList& OutActors) const;

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
    static void HandleTimeAdvanced(IAvalonUnit* Listener, long HoursAdvanced);
    /****************************************************************************************/

    /***************************************************************************************
    *  Actor Memory Management
    ****************************************************************************************/
public:
    void LoadActor(HardUnitRef& OutNewActor, HardUnitRef& Owner, FSaveContext& Context);
    /****************************************************************************************/
};
