/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "GameCalendar.h"
#include "../AvalonGameState.h"


#define REAL_SECONDS_PER_ADVANCE 1
#define GAMEHOURS_PER_ADVANCE 1

/***************************************************************************************
*   The Calendar Instance
****************************************************************************************/
/*static*/ void GameCalendar::LoadCalendar(HardUnitRef& OutGameCalendarRef, const FGameTime& Time)
{
	AvalonMemory::NewUnit<GameCalendar>(OutGameCalendarRef);
	GameCalendar* Calendar = IAvalonUnit::Get<GameCalendar>(OutGameCalendarRef);
	Calendar->mCurrentTime = Time;
}

/*static*/ void GameCalendar::Tick(float DeltaSeconds)
{
	GameCalendar* Calendar = GetCalendarInstance();
	if (Calendar->IsTimeAdvancing())
	{
		Calendar->mSecondsAdvancing += DeltaSeconds;
		if (Calendar->mSecondsAdvancing > REAL_SECONDS_PER_ADVANCE)
		{
			int TimeToAdvance = GAMEHOURS_PER_ADVANCE;

			FGameTime Delta = FGameTime();
			Delta.mTimeStamp[ETimeScale::ETS_Hours].mValue = TimeToAdvance;

			Calendar->mCurrentTime.AddTime(Delta);

			long DeltaHours = Delta.GetAsHours();
			Calendar->mOnTimeAdvanced.BroadcastEvent(DeltaHours);

			Calendar->mSecondsAdvancing = 0.f;
		}
	}
}
/****************************************************************************************/

/*static*/ void GameCalendar::SetWantsAdvanceTime(void* Asker, bool AdvanceTime)
{
	GameCalendar* Calendar = GetCalendarInstance();
	if (Calendar != nullptr)
	{
		Calendar->SetWantsAdvanceTime_Internal(Asker, AdvanceTime);
	}
}

void GameCalendar::SetWantsAdvanceTime_Internal(void* Asker, bool AdvanceTime)
{
	auto It = mRefHandles.find(Asker);
	if (It != mRefHandles.end())
	{
		if (It->second == AdvanceTime)
		{
			return;
		}
	}

	mRefHandles[Asker] = AdvanceTime;

	if (AdvanceTime)
	{
		mAdvanceTimeRefCount++;
	}
	else
	{
		mAdvanceTimeRefCount--;
	}
}

/*static*/ bool GameCalendar::IsTimeAdvancing()
{
	GameCalendar* Calendar = GetCalendarInstance();
	if (Calendar != nullptr)
	{
		return Calendar->IsTimeAdvancing_Internal();
	}

	return false;
}

bool GameCalendar::IsTimeAdvancing_Internal()
{
	return mAdvanceTimeRefCount > 0;
}

/*static*/ FGameTime& GameCalendar::GetCurrentTime()
{
	return GetCalendarInstance()->mCurrentTime;
}

/*static*/ void GameCalendar::BindEvent_TimeAdvanced(IAvalonUnit* Listener, GameTimeEvent::Callback& Callback)
{
	GameCalendar* Calendar = GetCalendarInstance();
	if (Calendar != nullptr)
	{
		Calendar->mOnTimeAdvanced.BindEvent(Listener, Callback);
	}
}

/*static*/ void GameCalendar::UnBindEvent_TimeAdvanced(IAvalonUnit* Listener)
{
	GameCalendar* Calendar = GetCalendarInstance();
	if (Calendar != nullptr)
	{
		Calendar->mOnTimeAdvanced.UnBindEvent(Listener);
	}
}

/*static*/ GameCalendar* GameCalendar::GetCalendarInstance()
{
	return AvalonGameState::GetGameState().GetGameCalendar();
}
/****************************************************************************************/
