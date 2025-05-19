/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include <string>

enum ETimeScale
{
    ETS_Hours   = 0,
    ETS_Days    = 1,
    ETS_Months  = 2,
    ETS_Years   = 3,
    ETS_Count
};

enum ETimeOfDay
{
    ETOD_Dawning    = 0,
    ETOD_Forenoon   = 1,
    ETOD_Noontide   = 2,
    ETOD_Eventide   = 3,
    ETOD_Gloaming   = 4,
    ETOD_Fulldark   = 5,
    ETOD_Count
};

struct FTimeScaleValue
{
    ETimeScale mScale;
    int mValue = 0;

    int Flatten();

    FTimeScaleValue& operator+=(const FTimeScaleValue& rhs);

    friend bool operator<(const FTimeScaleValue& lhs, const FTimeScaleValue& rhs);
    friend inline bool operator> (const FTimeScaleValue& lhs, const FTimeScaleValue& rhs) { return rhs < lhs; }
    friend inline bool operator<=(const FTimeScaleValue& lhs, const FTimeScaleValue& rhs) { return !(lhs > rhs); }
    friend inline bool operator>=(const FTimeScaleValue& lhs, const FTimeScaleValue& rhs) { return !(lhs < rhs); }

    friend FTimeScaleValue operator+(FTimeScaleValue lhs, const FTimeScaleValue& rhs)
    {
        lhs + rhs;
        return lhs;
    }

    friend inline bool operator==(const FTimeScaleValue& lhs, const FTimeScaleValue& rhs)
    { 
        return lhs.mValue == rhs.mValue;
    }
};

struct FSaveContext;
struct FGameTime
{
    void Load(const char* ID, FSaveContext& Context);
    void Save(const char* ID, FSaveContext& Context);

    FTimeScaleValue mTimeStamp[ETimeScale::ETS_Count];

    void AddTime(const FGameTime& Delta);

    long GetAsHours() const;
    long GetDeltaHours(const FGameTime& rhs) const;

    bool operator<(const FGameTime& rhs) const;
};

namespace GameTimeUtils
{
    const char* GetMonthName(int Month);
    std::string GetDayString(int Day);
    std::string GetTimeOfDayString(ETimeOfDay TimeOfDay);
    std::string GetTimeOfDayString(const FGameTime& InTime);
    void GetTimeOfDay(const FGameTime& InTime, ETimeOfDay& OutTOD, float& OutProgress);
}

