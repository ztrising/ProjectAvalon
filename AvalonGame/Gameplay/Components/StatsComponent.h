/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../Actor/ActorTypes.h"
#include "../Actions/AvalonAction.h"
#include "../Utility/StatTypes.h"
#include "../Utility/EventDispatcher.h"

#include <vector>
#include <map>

struct FStatusEffect;

/***************************************************************************************
*  Stats Component - Gives an Actor 'Stats'
****************************************************************************************/
class StatsComponent : public IActorComponent
{
public:
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    void AdvanceTime(long DeltaHours) override;

    void ApplyStatusEffect(FStatusEffect* StatusEffect);
    void RemoveStatusEffect(FStatusEffect* StatusEffect);

    std::map<std::string, HardUnitRef> mStats;
   // std::map<std::string, FAvalonStat*> mStats;

private:
    void ApplyStatDamage(std::string StatID, float Damage);
    std::vector<FStatusEffect*> mStatusEffects;

    /***************************************************************************************
    *   Stat Events!
    ****************************************************************************************/
public:
    typedef FEventDispatcher<std::string> StatsEvent;
    StatsEvent mOnStatChanged;
    /****************************************************************************************/
};

/***************************************************************************************
*  Status Effects
****************************************************************************************/
struct FStatusEffect : public IAvalonEffect
{
    void ExecuteEffect(FEffectContext& Context) override;
    float CalculateDamage(long DeltaHours) const;

    std::string mStatID; // ex: "Health"

    std::vector<FAttributeMod*> mMinValueMods;
    std::vector<FAttributeMod*> mMaxValueMods;

    FAvalonAttribute* mEffectLevel = nullptr;
    int mDuration = -1;

    float mBaseDamage = 0.f;
};
