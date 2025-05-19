/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "GameTimeTypes.h"
#include "../Utility/Saveable.h"


#include <sstream>
#include <algorithm>

static const int TimeScaleConversions[ETS_Count] = { 24, 30, 12, -1 };

float GetTimeScaleConversion(ETimeScale From, ETimeScale To)
{
    if (From == To)
    {
        return 1.f;
    }

    // Converting down (Days to Hours)
    if (To < From)
    {
        float RetValue = 1;
        for (int i = To; i < From; ++i)
        {
            RetValue *= TimeScaleConversions[i];
        }

        return RetValue;
    }

    // Converting up (Hours to Days), this is problematic
    float RetValue = 1;
    for (int i = From; i < To; ++i)
    {
        RetValue /= TimeScaleConversions[i];
    }
}

const char* GetDayOfTheWeekName(int Day)
{
    switch (Day)
    {
    case 0:  return "Sunday";
    case 1:  return "Monday";
    case 2:  return "Tuesday";
    case 3:  return "Wednesday";
    case 4:  return "Thursday";
    case 5:  return "Friday";
    case 6:  return "Saturday";
    }

    return "DefaultDayText";
}

const char* GameTimeUtils::GetMonthName(int Month)
{
    switch (Month)
    {
    case 0:  return "January";
    case 1:  return "February";
    case 2:  return "March";
    case 3:  return "April";
    case 4:  return "May";
    case 5:  return "June";
    case 6:  return "July";
    case 7:  return "August";
    case 8:  return "September";
    case 9:  return "October";
    case 10: return "November";
    case 11: return "December";
    }

    return "DefaultMonthText";
}



int FTimeScaleValue::Flatten()
{
    int Rollover = 0;

    int MaxValue = TimeScaleConversions[mScale];
    if (MaxValue > 0 && mValue >= MaxValue)
    {
        Rollover += mValue / MaxValue;
        mValue = mValue % MaxValue;
    }

    return Rollover;
}

FTimeScaleValue& FTimeScaleValue::operator+=(const FTimeScaleValue& rhs)
{
    mValue += (rhs.mValue * GetTimeScaleConversion(rhs.mScale, mScale));
    return *this;
}

bool operator< (const FTimeScaleValue& lhs, const FTimeScaleValue& rhs)
{
    return lhs.mValue < (rhs.mValue* GetTimeScaleConversion(rhs.mScale, lhs.mScale));
}


void FGameTime::Load(const char* ID, FSaveContext& Context)
{
    std::string TimeString;
    Context.Load(ID, TimeString);

    std::vector<std::string> ScaleStrings;
    {
        std::stringstream stream(TimeString);
        std::string ScaleString;
        while (std::getline(stream, ScaleString, '.'))
        {
            ScaleStrings.push_back(ScaleString);
        }
    }


    if (ScaleStrings.size() != ETS_Count)
    {
        return;
    }

    const int LastValue = ETS_Count - 1;
    for (int TimeScale = 0; TimeScale < ETS_Count; ++TimeScale)
    {
        FTimeScaleValue& Value = mTimeStamp[TimeScale];
        Value.mScale = ETimeScale(TimeScale);
        Value.mValue = std::atoi(ScaleStrings[LastValue - TimeScale].c_str());
    }
}

void FGameTime::Save(const char* ID, FSaveContext& Context)
{
    // TODO
    //FTimeScaleValue mTimeStamp[ETimeScale::ETS_Count];
}

void FGameTime::AddTime(const FGameTime& Delta)
{
    for (unsigned int TimeScale = 0; TimeScale < ETS_Count; ++TimeScale)
    {
        FTimeScaleValue& Value = mTimeStamp[TimeScale];
        Value += Delta.mTimeStamp[TimeScale];

        int Rollover = Value.Flatten();
        if (Rollover > 0 && TimeScale < (ETS_Count - 1))
        {
            mTimeStamp[TimeScale + 1].mValue += Rollover;
        }
    }
}

long FGameTime::GetAsHours() const
{
    long RetValue = 0;

    for (int TimeScale = 0; TimeScale < ETS_Count; ++TimeScale)
    {
        const FTimeScaleValue& Value = mTimeStamp[TimeScale];
        RetValue += (Value.mValue * GetTimeScaleConversion(ETimeScale(TimeScale), ETS_Hours));
    }
    return RetValue;
}

long FGameTime::GetDeltaHours(const FGameTime& rhs) const
{
    return GetAsHours() - rhs.GetAsHours();
}

bool FGameTime::operator<(const FGameTime& rhs) const
{
    bool RequireLessThan = false;
    for (int TimeScale = ETS_Count - 1; TimeScale >= 0; --TimeScale)
    {
        const FTimeScaleValue& Value = mTimeStamp[TimeScale];
        const FTimeScaleValue& CheckValue = rhs.mTimeStamp[TimeScale];

        if (Value > CheckValue)
        {
            return false;
        }

        if (Value == CheckValue)
        {
            if (RequireLessThan)
            {
                return false;
            }

            RequireLessThan = true;
        }
        else
        {
            RequireLessThan = false;
        }
    }

    return true;
}

std::string GameTimeUtils::GetDayString(int Day)
{
    int DisplayDay = Day + 1;

    std::string DayNumber = std::to_string(DisplayDay);

    std::string Postfix;
    int PostFixType = DisplayDay - (DisplayDay / 10) * 10;
    switch (PostFixType)
    {
    case 1:     Postfix = "st"; break;
    case 2:     Postfix = "nd"; break;
    case 3:     Postfix = "rd"; break;
    default:    Postfix = "th"; break;
    }

    return DayNumber + Postfix;
}

std::string GameTimeUtils::GetTimeOfDayString(ETimeOfDay TimeOfDay)
{
    std::string RetValue = "INVALID_TIME_OF_DAY";

    switch (TimeOfDay)
    {
    case ETimeOfDay::ETOD_Dawning:      RetValue = "Dawning"; break;
    case ETimeOfDay::ETOD_Forenoon:     RetValue = "Forenoon"; break;
    case ETimeOfDay::ETOD_Noontide:     RetValue = "Noontide"; break;
    case ETimeOfDay::ETOD_Eventide:     RetValue = "Eventide"; break;
    case ETimeOfDay::ETOD_Gloaming:     RetValue = "Gloaming"; break;
    case ETimeOfDay::ETOD_Fulldark:     RetValue = "Fulldark"; break;
    }

    return RetValue;
}

std::string GameTimeUtils::GetTimeOfDayString(const FGameTime& InTime)
{
    ETimeOfDay TimeOfDay;
    float Progress;
    GetTimeOfDay(InTime, TimeOfDay, Progress);

    return GetTimeOfDayString(TimeOfDay);
}

void GameTimeUtils::GetTimeOfDay(const FGameTime& InTime, ETimeOfDay& OutTOD, float& OutProgress)
{
    static const float HoursPerTimeOfDay = 4.f;
    static const int MIN_VALUE[ETimeOfDay::ETOD_Count] = { 4, 8, 12, 16, 20, 0 };
    static const int MAX_VALUE[ETimeOfDay::ETOD_Count] = { 8, 12, 16, 20, 24, 4 };

    int Hour = InTime.mTimeStamp[ETimeScale::ETS_Hours].mValue;

    for (int i = 0; i < ETimeOfDay::ETOD_Count; ++i)
    {
        if (Hour >= MIN_VALUE[i] && Hour < MAX_VALUE[i])
        {
            OutTOD = ETimeOfDay(i);
            OutProgress = float(Hour - MIN_VALUE[i]) / HoursPerTimeOfDay;
            return;
        }
    }

    // ERROR!
    OutTOD = ETimeOfDay::ETOD_Count;
    OutProgress = 0;
}