/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "GameCalendar.h"

#define REAL_SECONDS_PER_ADVANCE 1
#define GAMEHOURS_PER_ADVANCE 1

GameCalendar* GameCalendar::mGameCalendar = nullptr;

/***************************************************************************************
*   The Calendar Instance
****************************************************************************************/
/*static*/ GameCalendar* GameCalendar::Get()
{
	return GameCalendar::mGameCalendar;
}

/*static*/ void GameCalendar::LoadCalendar(const FGameTime& Time)
{
	GameCalendar::mGameCalendar = new GameCalendar();
	GameCalendar::mGameCalendar->mCurrentTime = Time;
}

void GameCalendar::Tick(float DeltaSeconds)
{
	if (IsTimeAdvancing())
	{
		mSecondsAdvancing += DeltaSeconds;
		if (mSecondsAdvancing > REAL_SECONDS_PER_ADVANCE)
		{
			int TimeToAdvance = GAMEHOURS_PER_ADVANCE;

			FGameTime Delta = FGameTime();
			Delta.mTimeStamp[ETimeScale::ETS_Hours].mValue = TimeToAdvance;

			mCurrentTime.AddTime(Delta);

			long DeltaHours = Delta.GetAsHours();
			mOnTimeAdvanced.BroadcastEvent(DeltaHours);

			mSecondsAdvancing = 0.f;
		}
	}
}
/****************************************************************************************/

/*static*/ void GameCalendar::SetWantsAdvanceTime(void* Asker, bool AdvanceTime)
{
	if (GameCalendar::mGameCalendar != nullptr)
	{
		GameCalendar::mGameCalendar->SetWantsAdvanceTime_Internal(Asker, AdvanceTime);
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
	if (GameCalendar::mGameCalendar != nullptr)
	{
		return GameCalendar::mGameCalendar->IsTimeAdvancing_Internal();
	}

	return false;
}

bool GameCalendar::IsTimeAdvancing_Internal()
{
	return mAdvanceTimeRefCount > 0;
}

/*static*/ FGameTime& GameCalendar::GetCurrentTime()
{
	return GameCalendar::mGameCalendar->mCurrentTime;
}

/*static*/ void GameCalendar::BindEvent_TimeAdvanced(IEventListener* Listener, GameTimeEvent::Callback& Callback)
{
	if (GameCalendar::mGameCalendar != nullptr)
	{
		GameCalendar::mGameCalendar->mOnTimeAdvanced.BindEvent(Listener, Callback);
	}
}

/*static*/ void GameCalendar::UnBindEvent_TimeAdvanced(IEventListener* Listener)
{
	if (GameCalendar::mGameCalendar != nullptr)
	{
		GameCalendar::mGameCalendar->mOnTimeAdvanced.UnBindEvent(Listener);
	}
}
/****************************************************************************************/
