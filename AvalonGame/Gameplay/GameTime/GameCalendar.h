/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "GameTimeTypes.h"
#include "../Utility/EventDispatcher.h"

#include <map>

class GameCalendar : public IAvalonUnit
{
public:
    static void LoadCalendar(HardUnitRef& OutGameCalendarRef, const FGameTime& Time);
    static void Tick(float DeltaSeconds);

    /***************************************************************************************
    *   Time Events!
    ****************************************************************************************/
public:
    typedef FEventDispatcher<long> GameTimeEvent;
private:
    GameTimeEvent mOnTimeAdvanced;
    /****************************************************************************************/

    /***************************************************************************************
    *   Advancing Time
    ****************************************************************************************/
public:
    static void SetWantsAdvanceTime(void* Asker, bool AdvanceTime);
    static void BindEvent_TimeAdvanced(IAvalonUnit* Listener, GameTimeEvent::Callback& Callback);
    static void UnBindEvent_TimeAdvanced(IAvalonUnit* Listener);
    static FGameTime& GetCurrentTime();
    static bool IsTimeAdvancing();

private:
    void SetWantsAdvanceTime_Internal(void* Asker, bool AdvanceTime);
    bool IsTimeAdvancing_Internal();

    static GameCalendar* GetCalendarInstance();

    int mAdvanceTimeRefCount = 0;
    std::map<void*, bool> mRefHandles;

    float mSecondsAdvancing = 0;
    FGameTime mCurrentTime;
    /****************************************************************************************/
};

