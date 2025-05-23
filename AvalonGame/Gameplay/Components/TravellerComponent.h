/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../Actor/ActorTypes.h"
#include "../Map/TravelTypes.h"
#include "../Actions/AvalonAction.h"
#include "../Utility/EventDispatcher.h"

/***************************************************************************************
*   Traveller Component - Allows an Actor to Travel between Locations
****************************************************************************************/
class Traveller : public IActorComponent
{
public:
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    void RequestTravel(const FPathRequest& Request);
    void FinishTravel();

    void AdvanceTime(long DeltaHours) override;

    bool IsTravelling() { return mPath != nullptr; }

    FTravelPath* GetPath() const { return mPath; }
    const FPathRequest& GetPathRequest() const { return mPathDef; }
    float GetProgress() const { return mProgress; }

private:
    FTravelPath* mPath = nullptr;
    float mProgress = 0.f;
    bool mPathDirection = false;

    // For Save/Load during travel
    FPathRequest mPathDef;

    /***************************************************************************************
    *   Travel Events!
    ****************************************************************************************/
public:
    typedef FEventDispatcher<Traveller*> TravelEvent;

    TravelEvent mOnTravelBegin;
    TravelEvent mOnTravelEnd;
    /****************************************************************************************/
};

/***************************************************************************************
*   Portal Component - Connections to other Locations
****************************************************************************************/
class PortalComponent : public IActorComponent, public IActionProvider
{
public:
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    void GatherActionsFor(const AvalonActor* Target, ActionList& OutActions) override;

    FPathRequest mRequest;
    std::string mTravelPrompt;
};

/***************************************************************************************
*   Travel Effect
****************************************************************************************/
struct Effect_Travel : public IAvalonEffect
{
    void ExecuteEffect(FEffectContext& Context) override;
};
