/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Meditator.h"
#include "../GameTime/GameCalendar.h"

/***************************************************************************************
*  Meditator Component!  One who meditates!
****************************************************************************************/
void Meditator::Load(FSaveContext& Context)
{

}

void Meditator::Save(FSaveContext& Context)
{

}

void Meditator::AdvanceTime(long DeltaHours)
{
	if (mIsMeditating)
	{
		mMeditatingHoursLeft -= DeltaHours;
		if (mMeditatingHoursLeft <= 0)
		{
			StopMeditating();
		}
	}
}

void Meditator::StartMeditating(int DeltaHours)
{
	if (DeltaHours > 0)
	{
		mMeditatingHoursLeft = DeltaHours;
		mIsMeditating = true;

		GameCalendar::SetWantsAdvanceTime(this, true);

		// TODO:  Apply status regens
	}
}

void Meditator::StopMeditating()
{
	if (IsMeditating())
	{
		mMeditatingHoursLeft = 0;
		mIsMeditating = false;

		GameCalendar::SetWantsAdvanceTime(this, false);

		// TODO: Remove status regens
	}
}
