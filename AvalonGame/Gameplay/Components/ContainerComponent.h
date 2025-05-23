/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../Actor/ActorTypes.h"
#include "../Utility/AvalonTags.h"
#include "../Actions/AvalonAction.h"
#include "../Utility/EventDispatcher.h"

#include <vector>
#include <map>

class IContainer : public IActorComponent
{
public:
    virtual void GetContainerContents(HardRefList& OutContents) const = 0;

    void GetContainersWithin(ContainerList& OutContainers);
    bool IsInContainer(const AvalonActor* InActor) const;
    virtual bool CanAddToContainer(const AvalonActor* InActor) const;
    virtual void AddToContainer(AvalonActor* ToAdd);
    virtual void RemoveFromContainer(HardUnitRef& ToRemove);
    virtual void GetItemPouchContainers(ContainerList& OutContainers) {}
};

/***************************************************************************************
*  Level Container - Levels hold Actors
****************************************************************************************/
class LevelContainer : public IContainer
{
public:
    virtual void Load(FSaveContext& Context) override;
    virtual void Save(FSaveContext& Context) override;

    /***************************************************************************************
    *  IActorComponent
    ****************************************************************************************/
public:
    void Tick(float DeltaSeconds) override;
    void AdvanceTime(long DeltaHours) override;
    /***************************************************************************************/

    /***************************************************************************************
    *  IContainer
    ****************************************************************************************/
public:
    void GetContainerContents(HardRefList& OutContents) const override;
    
    bool CanAddToContainer(const AvalonActor* Actor) const override;
    void AddToContainer(AvalonActor* ToAdd) override;
    void RemoveFromContainer(HardUnitRef& ToRemove) override;
    /***************************************************************************************/

private:

    // Levels hold an ownership reference to actors directly in the level
    std::vector<HardUnitRef> mLevelActors;
};

/***************************************************************************************
*  Item Container - Like a bag! Has slots!
****************************************************************************************/
class ItemContainer : public IContainer, public IActionProvider
{
public:
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    void GatherActionsFor(const AvalonActor* Target, ActionList& OutActions) override;

    void OpenContainer();
    void CloseContainer();

    /***************************************************************************************
    *  IContainer
    ****************************************************************************************/
    void GetContainerContents(HardRefList& OutContents) const override;
    
    bool CanAddToContainer(const AvalonActor* Actor) const override;
    void AddToContainer(AvalonActor* ToAdd) override;
    void RemoveFromContainer(HardUnitRef& ToRemove) override;
    void GetItemPouchContainers(ContainerList& OutContainers) override;
    /***************************************************************************************/

protected:

    FAvalonTagQuery mFilter;
    HardRefList mContents;

    bool mIsOpen = false;

    /***************************************************************************************
    *  UI Helpers
    ****************************************************************************************/
public:
    // TODO:  All this, but like, better
    HardUnitRef GetContentsByIndex(int Index);
    std::string GetDisplayName();
    int GetNumItems();
    int GetMaxSlots();
};

/***************************************************************************************
*  Container Effects
****************************************************************************************/
struct Effect_OpenContainer : public IAvalonEffect
{
    void ExecuteEffect(FEffectContext& Context) override;
    IContainer* mContainer;
};

struct Effect_CloseContainer : public IAvalonEffect
{
    void ExecuteEffect(FEffectContext& Context) override;
    IContainer* mContainer;
};

/***************************************************************************************
*  Looter Component - I can open Containers and move Lootables around!
****************************************************************************************/
class LooterComponent : public IActorComponent
{
public:
    void Load(FSaveContext& Context) override {}
    void Save(FSaveContext& Context) override {}

    void OpenContainer(IContainer* Container);
    void CloseContainer(IContainer* Container);

    ContainerList mOpenContainers;

    /***************************************************************************************
    *   Open/Close Container Events!
    ****************************************************************************************/
public:
    typedef FEventDispatcher<IContainer*> LooterEvent;

    LooterEvent mOnContainerOpened;
    LooterEvent mOnContainerClosed;
    /****************************************************************************************/
};

/***************************************************************************************
*  Equipment Component - I can equip things!
****************************************************************************************/
struct FEquipSlot
{
    void Load(FSaveContext& Context, IActorComponent* Parent);
    void Save(FSaveContext& Context);

    std::string mID;
    int mMaxCapacity = 1;
    HardRefList mSlotContents;
};

class Equipment : public IContainer
{
public:
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    const FEquipSlot* GetSlotByID(std::string ID) const;
    FEquipSlot* GetSlotByID(std::string ID);

    /***************************************************************************************
    *  IContainer
    ****************************************************************************************/
    void GetContainerContents(std::vector<HardUnitRef>& OutContents) const override;
    
    bool CanAddToContainer(const AvalonActor* Actor) const override;
    void AddToContainer(AvalonActor* ToAdd) override;
    void RemoveFromContainer(HardUnitRef& Actor) override;
    void GetItemPouchContainers(std::vector<IContainer*>& OutContainers) override;
    /***************************************************************************************/

    std::vector<FEquipSlot> mEquipment;
};

/***************************************************************************************
*  Fastening! - Fastening is Conceptually tying things to other things!
****************************************************************************************/
/*class FastenerSlotComponent : public IActorComponent
{
public:
};*/
/***************************************************************************************/

/***************************************************************************************
*  Trade Actions
****************************************************************************************/
/*struct Effect_StartTrade : public IContainerEffect
{
    void ExecuteEffect() override;
};

struct Effect_StopTrade : public IContainerEffect
{
    void ExecuteEffect() override;
};

struct Effect_ConfirmTrade : public IContainerEffect
{
    void ExecuteEffect() override;
};*/
/****************************************************************************************/



