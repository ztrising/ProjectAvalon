/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../Utility/Saveable.h"
#include "../../AvalonUnit/AvalonUnit.h"

class AvalonActor;

class IActorComponent : public ISaveable
{
public:
    virtual void Tick(float DeltaSeconds) {};
    virtual void AdvanceTime(long DeltaHours) {};

    void SetActorOwner(const FUnitHandle Value) { mActorOwnerHandle = Value; }
    FUnitHandle& GetActorOwnerHandle() { return mActorOwnerHandle;}
    const FUnitHandle& GetActorOwnerHandleConst() const { return mActorOwnerHandle;}
    AvalonActor* GetActorOwner() const { return mActorOwnerHandle.Get<AvalonActor>(); }
    const AvalonActor* GetActorOwnerConst() const { return GetActorOwner(); }
    FUnitHandle LoadChildActor(FSaveContext& Context);

private:
    FUnitHandle mActorOwnerHandle;
};

class ActorComponentFactory
{
    std::list<IActorComponent> mDefaultComponentInstances;
};
