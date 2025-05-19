/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "GameTimeTypes.h"
#include "../Utility/EventDispatcher.h"

#include <map>

class GameCalendar
{
    /***************************************************************************************
    *   The Calendar Instance
    ****************************************************************************************/
public:
    static GameCalendar* Get();
    static void LoadCalendar(const FGameTime& Time);
    void Tick(float DeltaSeconds);

private:
    static GameCalendar* mGameCalendar;
    /****************************************************************************************/

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
    static void BindEvent_TimeAdvanced(IEventListener* Listener, GameTimeEvent::Callback& Callback);
    static void UnBindEvent_TimeAdvanced(IEventListener* Listener);
    static FGameTime& GetCurrentTime();
    static bool IsTimeAdvancing();

private:
    void SetWantsAdvanceTime_Internal(void* Asker, bool AdvanceTime);
    bool IsTimeAdvancing_Internal();

    int mAdvanceTimeRefCount = 0;
    std::map<void*, bool> mRefHandles;

    float mSecondsAdvancing = 0;
    FGameTime mCurrentTime;
    /****************************************************************************************/
};

