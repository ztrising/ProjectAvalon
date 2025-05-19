/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once
/*

#include "Attributes.h"
#include "../Actions/AvalonAction.h"

enum EPlayerStats
{
    EHealth,
    EStamina,
    EHunger,
    EThirst,
	EToxicity,
    EPlayerStatsCount,
};

struct FPlayerStat
{
    float mCurrentValue;
    AvalonAttributes::FAttribute mMaxValueAttribute;
    bool mChangedThisFrame = false;
};

struct FPlayerAttribueMod
{ 
    EPlayerStats mAttributeToMod;
    AvalonAttributes::FAttributeMod mMod;
};

struct Effect_StatDamage : public IAvalonEffect
{
    void ExecuteEffect() override;

    EPlayerStats mStat = EPlayerStats::EHealth;
    float mDamage = 0.f;
};

// struct Effect_OngoingStatChange : public IAvalonEffect
// struct Effect_MaxStatChange : public IAvalonEffect


struct ISideEffect
{
    virtual void OnApplied() = 0;
    virtual void OnRemoved() = 0;

    // Returns true when finished
    virtual bool Tick(float DeltaTime) = 0;

    short mDurationInHours = 0;
};

struct SideEffect_MaxStatChange : public ISideEffect
{

    DynamicArray<FPlayerAttribueMod*> mAttributeMods;
};


class IPlayerStatListener
{
public:
	// Sent out once per frame if any amount of changes were made to this stat
	virtual void HandleStatValueChange(EPlayerStats Stat, float Value, float MaxValue) = 0;
};

void ListenForStatValueChanges(IPlayerStatListener* Listener);
void UnlistenForStatValueChanges(IPlayerStatListener* Listener);

*/

