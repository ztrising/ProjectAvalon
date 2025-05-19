/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "PlayerState.h"

/* void PlayerState::LoadFromSave(const char* SaveFile)
{
	// Simulate a load
	mStats[EPlayerStats::EHealth].mCurrentValue = 120;
	mStats[EPlayerStats::EHealth].mMaxValueAttribute.InitValue(150);

	mStats[EPlayerStats::EStamina].mCurrentValue = 80;
	mStats[EPlayerStats::EStamina].mMaxValueAttribute.InitValue(120);

	mStats[EPlayerStats::EHunger].mCurrentValue = 90;
	mStats[EPlayerStats::EHunger].mMaxValueAttribute.InitValue(100);

	mStats[EPlayerStats::EThirst].mCurrentValue = 75;
	mStats[EPlayerStats::EThirst].mMaxValueAttribute.InitValue(100);

	mStats[EPlayerStats::EToxicity].mCurrentValue = 15;
	mStats[EPlayerStats::EToxicity].mMaxValueAttribute.InitValue(50);

	unsigned int NumListeners = mListeners.Num();
	unsigned int NumStats = EPlayerStats::EPlayerStatsCount;
	for (unsigned int ListenerIndex = 0; ListenerIndex < NumListeners; ++ListenerIndex)
	{
		for (unsigned int StatsIndex = 0; StatsIndex < NumStats; ++StatsIndex)
		{
			EPlayerStats Stat = static_cast<EPlayerStats>(StatsIndex);
			float CurrentValue = mStats[Stat].mCurrentValue;
			float MaxValue = mStats[Stat].mMaxValueAttribute.GetValue();

			mListeners[ListenerIndex]->HandleStatValueChange(Stat, CurrentValue, MaxValue);
		}
	}

	AvalonListeners::ListenForTimeChange(this);
}

void PlayerState::Tick(float DeltaTime)
{
	unsigned int NumStats = EPlayerStats::EPlayerStatsCount;
	for (unsigned int StatsIndex = 0; StatsIndex < NumStats; ++StatsIndex)
	{
		EPlayerStats Stat = static_cast<EPlayerStats>(StatsIndex);
		if (mStats[Stat].mChangedThisFrame)
		{
			mStats[Stat].mChangedThisFrame = false;
			float CurrentValue = mStats[Stat].mCurrentValue;
			float MaxValue = mStats[Stat].mMaxValueAttribute.GetValue();

			unsigned int NumListeners = mListeners.Num();
			for (unsigned int ListenerIndex = 0; ListenerIndex < NumListeners; ++ListenerIndex)
			{
				mListeners[ListenerIndex]->HandleStatValueChange(Stat, CurrentValue, MaxValue);
			}
		}
	}
}

void PlayerState::DealDamageToStat(EPlayerStats eStat, float Damage)
{
	mStats[eStat].mCurrentValue -= Damage;
	mStats[eStat].mChangedThisFrame = true;
}

void PlayerState::HandleTimeAdvanced(const FGameTime& CurrentTime, const FGameTime& TimeDelta)
{
	short DeltaHours = TimeDelta.GetAsHours();
	// Hunger value decreases 1 per hour
	DealDamageToStat(EPlayerStats::EHunger, (5.f * DeltaHours));

	// Thirst value decreases 2 per hour
	DealDamageToStat(EPlayerStats::EThirst, (10.f * DeltaHours));
}

void ListenForStatValueChanges(IPlayerStatListener* Listener)
{
	PlayerState::GetPlayerState().mListeners.Add(Listener);
}

void UnlistenForStatValueChanges(IPlayerStatListener* Listener)
{
	PlayerState::GetPlayerState().mListeners.Remove(Listener);
}

void Effect_StatDamage::ExecuteEffect()
{
	PlayerState::GetPlayerState().DealDamageToStat(mStat, mDamage);
}
*/