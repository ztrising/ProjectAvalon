/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "StatsComponent.h"
#include "../Actor/AvalonActor.h"

#include <string>

/***************************************************************************************
*  Stats Component - Gives an Actor 'Stats'
****************************************************************************************/
void StatsComponent::Load(FSaveContext& Context)
{
    // Expected Layout:
    /*
        <Stats>
            <Health ... />
            <Stamina ... />
            <Hydration ... />
            <Fullness ../>
        </Stats>
    */

	auto StatsFactory_Lambda = [&](HardUnitRef& OutNewStat, FSaveContext& Context)
	{

        AvalonMemory::NewUnit<FAvalonStat>(OutNewStat);

        FAvalonStat* NewStat = Get<FAvalonStat>(OutNewStat);
        ISaveable::Load(NewStat, Context);

        std::string StatID = Context.GetSaveID();
        mStats[StatID] = OutNewStat;
	};

    HardRefList Cup;
    Context.AllocateChildrenWithFactory(Cup, StatsFactory_Lambda);
}

void StatsComponent::Save(FSaveContext& Context)
{
    for (auto& Entry : mStats)
    {
        FAvalonStat* Stat = Get<FAvalonStat>(Entry.second);
        ISaveable::Save(Stat, Context);
    }
}

void StatsComponent::AdvanceTime(long DeltaHours)
{
    for (FStatusEffect* Effect : mStatusEffects)
    {
        float Damage = Effect->CalculateDamage(DeltaHours);
        ApplyStatDamage(Effect->mStatID, Damage);
    }

    // TODO: Remove when duration is exceeded

    for (auto StatPair : mStats)
    {
        FAvalonStat* Stat = Get<FAvalonStat>(StatPair.second);
        float RegenValue = Stat->mRegen.GetValue();
        if (RegenValue != 0.f)
        {
            float NewValue = Stat->mCurrentValue + RegenValue;
            Stat->SetCurrentValue(NewValue);
        }

        if (Stat->mChangedThisFrame)
        {
            mOnStatChanged.BroadcastEvent(StatPair.first);
            Stat->mChangedThisFrame = false;
        }
    }
}

void StatsComponent::ApplyStatDamage(std::string StatID, float Damage)
{
    auto search = mStats.find(StatID);
    if (search != mStats.end()) 
    {
        FAvalonStat* Stat = Get<FAvalonStat>(search->second);
        float NewValue = Stat->mCurrentValue + Damage;
        Stat->SetCurrentValue(NewValue);
    }
}

void StatsComponent::ApplyStatusEffect(FStatusEffect* StatusEffect)
{
    //TODO, apply mods

    mStatusEffects.push_back(StatusEffect);
}

void StatsComponent::RemoveStatusEffect(FStatusEffect* StatusEffect)
{
    // TODO, Remove mods
    // TODO, remove by value
}

/***************************************************************************************
*  Status Effects
****************************************************************************************/
void FStatusEffect::ExecuteEffect(FEffectContext& Context)
{
    AvalonActor* TargetActor = AvalonActor::Get<AvalonActor>(Context.mTarget);
    if (StatsComponent* StatsComp = TargetActor->GetComponent<StatsComponent>())
    {
        StatsComp->ApplyStatusEffect(this);
    }
}

float FStatusEffect::CalculateDamage(long DeltaHours) const
{
    float RetValue = mBaseDamage * DeltaHours;

    if (mEffectLevel != nullptr)
    {
        RetValue *= mEffectLevel->GetValue();
    }

    // Let target modify?  Based on defensive value?
    // Let source modify?  Based on ... attacking value?

    return RetValue;
}
