/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../Utility/Saveable.h"
#include "../../AvalonUnit/AvalonUnit.h"

class AvalonActor;
typedef std::vector<AvalonActor*> ActorList;

class IContainer;
typedef std::vector<IContainer*> ContainerList;


class IActorComponent : public ISaveable, public IAvalonUnit
{
public:
    virtual void Tick(float DeltaSeconds) {};
    virtual void AdvanceTime(long DeltaHours) {};

    void SetActorOwner(const HardUnitRef Value) { mOwner = Value; }
    AvalonActor* GetActorOwner() const { return Get<AvalonActor>(mOwner); }
    const AvalonActor* GetActorOwnerConst() const { return GetActorOwner(); }
    void LoadChildActor(HardUnitRef& ChildActor, FSaveContext& Context);

private:
    SoftUnitRef mOwner;
};

class ActorComponentFactory
{
    std::list<IActorComponent> mDefaultComponentInstances;
};
