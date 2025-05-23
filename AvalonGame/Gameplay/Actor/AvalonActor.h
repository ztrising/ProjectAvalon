/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "ActorTypes.h"
#include "../Utility/AvalonTags.h"
#include "../Actions/AvalonAction.h"
#include "../Map/TravelTypes.h"
#include "../Utility/EventDispatcher.h"
#include "../../AvalonUnit/AvalonUnit.h"

#include <vector>
#include <memory>

class LevelActor;

class AvalonActor : public IAvalonUnit, public ISaveable, public IActionProvider, public IEventListener
{
    /***************************************************************************************
    *  ISaveable
    ****************************************************************************************/
protected:
    virtual void Load(FSaveContext& Context) override;
    virtual void Save(FSaveContext& Context) override;
    /****************************************************************************************/

    /***************************************************************************************
    *  IActionProvider
    ****************************************************************************************/
public:
    virtual void GatherActionsFor(const AvalonActor* Target, ActionList& OutActions) override;
    /****************************************************************************************/

    /***************************************************************************************
    *  Basic Actor Properties
    ****************************************************************************************/
public:
    std::string mDisplayName;
    std::string mDescription;

    SoftUnitRef mLevelRef;

    FAvalonTagContainer mTags;
    
    virtual LevelActor* GetLevel();

    // Used by containers to set themselves as the owners of Actors
    // or reset the ownership when this actor is being removed from
    // a container.  (Levels are also containers)
    void SetOwner(HardUnitRef& InActor);
    void ResetOwner();
    
    // Check if the given InActorHandle is in the ownership chain of this actor
    bool IsOwnedBy(const AvalonActor* InActor);
    void GatherOwners(ActorList& OutOwners);

    virtual void Tick(float DeltaSeconds);
    virtual void AdvanceTime(long DeltaHours);

    void GetItemPouchContainers(ContainerList& OutContainers);

    void GatherContainers(ContainerList& OutContainers) const;
    FLocationInfo* GetLocation() const { return mLocation; }
    void SetLocation(FLocationInfo* NewLocation) { mLocation = NewLocation; }
protected:
    FLocationInfo* mLocation = nullptr;

private:
    SoftUnitRef mOwner;
    /****************************************************************************************/

    /***************************************************************************************
    *   Actor Events!
    ****************************************************************************************/
public:
    typedef FEventDispatcher<AvalonActor*> InventoryEvent;

    InventoryEvent mOnItemAdded;
    InventoryEvent mOnItemRemoved;
    /****************************************************************************************/

    /***************************************************************************************
    *  Actor Components
    ****************************************************************************************/
public:
    template <class T>
    T* GetComponent() const
    {
        for (IActorComponent* Component : mComponents)
        {
            if (T* RetValue = dynamic_cast<T*>(Component))
            {
                return RetValue;
            }
        }

        return nullptr;
    }

    template <class T>
    void GetComponents(std::vector<T*>& OutComponents) const
    {
        for (IActorComponent* Component : mComponents)
        {
            if (T* RetValue = dynamic_cast<T*>(Component))
            {
                OutComponents.push_back(RetValue);
            }
        }
    }

private:
    // TODO: make these shared ptrs, the actor should own the components
    std::vector<IActorComponent*> mComponents;
    /****************************************************************************************/
};
