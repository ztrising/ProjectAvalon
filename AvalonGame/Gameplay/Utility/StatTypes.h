/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Saveable.h"
#include "../Actions/AvalonAction.h"


#include <vector>
#include <map>

enum class EModOperation
{
    PreAdd,
    Multiply,
    Add,
    Override,
};

/***************************************************************************************
*   Attribute Mod
****************************************************************************************/
class FAttributeMod : public ISaveable
{
public:
    void ApplyModToValue(float& OutValue) const;
    bool operator<(const FAttributeMod& rhs) const;

private:
    EModOperation mOperation;
    float mValue;

    friend inline bool operator==(const FAttributeMod& lhs
        , const FAttributeMod& rhs)
    {
        return (lhs.mOperation == rhs.mOperation)
            && (lhs.mValue == rhs.mValue);
    }

    /***************************************************************************************
    *  ISaveable
    ****************************************************************************************/
private:
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;
    /****************************************************************************************/
};

/***************************************************************************************
*   Avalon Attribute
****************************************************************************************/
class FAvalonAttribute : public ISaveable
{
public:
    float GetValue();

    void AddModifier(FAttributeMod Mod);
    void RemoveModifier(FAttributeMod Mod);

private:
    void EvaluateAttribute();

    float mBaseValue = 0;
    float mCachedEvaluatedValue = 0;

    std::vector<FAttributeMod> mMods;

    /***************************************************************************************
    *  ISaveable
    ****************************************************************************************/
private:
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;
    /****************************************************************************************/
};

/***************************************************************************************
*   Avalon Stat
****************************************************************************************/

enum class EStatThresholdType
{
    Percentage_GreaterThan,
    Percentage_GreaterThanOrEqual,
    Percentage_LessThan,
    Percentage_LessThanOrEqual,
    Value_GreaterThan,
    Value_GreaterThanOrEqual,
    Value_LessThan,
    Value_LessThanOrEqual
};

struct FStatThreshold
{
    EStatThresholdType mType;
    float mThreshold = 1.f;

    AvalonAction* mThresholdAction = nullptr;
};

struct FAvalonStat : public ISaveable, public IAvalonUnit
{
public:
    std::string mStatID; // ex: "Health"

    float mCurrentValue;
    FAvalonAttribute mMinValue;
    FAvalonAttribute mMaxValue;
    FAvalonAttribute mRegen;

    bool mChangedThisFrame = false;
    std::vector<FStatThreshold*> mThresholdResponses;

    void SetCurrentValue(float NewValue);

private:
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;
};






