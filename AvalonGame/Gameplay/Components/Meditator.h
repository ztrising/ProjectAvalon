/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../Actor/ActorTypes.h"

/***************************************************************************************
*  Meditator Component!  One who meditates!
****************************************************************************************/
class Meditator : public IActorComponent
{
public:
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    void AdvanceTime(long DeltaHours) override;

    void StartMeditating(int DeltaHours);
    void StopMeditating();

    bool IsMeditating() const { return mIsMeditating; }
    
private:
    bool mIsMeditating;
    int mMeditatingHoursLeft = 0;
};
