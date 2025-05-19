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

class LevelActor;
class IContainer;

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
    virtual void GatherActionsFor(const FUnitHandle& Target, ActionList& OutActions) override;
    /****************************************************************************************/

    /***************************************************************************************
    *  Basic Actor Properties
    ****************************************************************************************/
public:
    std::string mDisplayName;
    std::string mDescription;

    FUnitHandle mLevel;

    FAvalonTagContainer mTags;
    
    virtual LevelActor* GetLevel();

    // Used by containers to set themselves as the owners of Actors
    // or reset the ownership when this actor is being removed from
    // a container.  (Levels are also containers)
    void SetOwner(const FUnitHandle& InActorHandle);
    void ResetOwner();
    
    // Check if the given InActorHandle is in the ownership chain of this actor
    bool IsOwnedBy(const FUnitHandle& InActorHandle);
    void GatherOwners(std::vector<FUnitHandle>& OutOwners);

    virtual void Tick(float DeltaSeconds);
    virtual void AdvanceTime(long DeltaHours);

    void GetItemPouchContainers(std::vector<IContainer*>& OutContainers);

    void GatherContainers(std::vector<IContainer*>& OutContainers);
    FLocationInfo* GetLocation() const { return mLocation; }
    void SetLocation(FLocationInfo* NewLocation) { mLocation = NewLocation; }
protected:
    FLocationInfo* mLocation = nullptr;

private:
    FUnitHandle mOwnerHandle;
    /****************************************************************************************/

    /***************************************************************************************
    *   Actor Events!
    ****************************************************************************************/
public:
    typedef FEventDispatcher<FUnitHandle&> InventoryEvent;

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
    std::vector<IActorComponent*> mComponents;
    /****************************************************************************************/
};
