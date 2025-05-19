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

class AvalonActor;

class IContainer : public IActorComponent
{
public:
    virtual void GetContainerContents(std::vector<FUnitHandle>& OutContents) const = 0;

    void GetContainersWithin(std::vector<IContainer*>& OutContainers);
    bool IsInContainer(const FUnitHandle& InHandle) const;
    virtual bool CanAddToContainer(const FUnitHandle& InHandle) const;
    virtual void AddToContainer(FUnitHandle& ToAddHandle);
    virtual void RemoveFromContainer(FUnitHandle& ToRemoveHandle);
    virtual void GetItemPouchContainers(std::vector<IContainer*>& OutContainers) {}
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
    void GetContainerContents(std::vector<FUnitHandle>& OutContents) const override;
    
    bool CanAddToContainer(const FUnitHandle& InHandle) const override;
    void AddToContainer(FUnitHandle& InHandle) override;
    void RemoveFromContainer(FUnitHandle& InHandle) override;
    /***************************************************************************************/

private:
    std::vector<FUnitHandle> mLevelActors;
};

/***************************************************************************************
*  Item Container - Like a bag! Has slots!
****************************************************************************************/
class ItemContainer : public IContainer, public IActionProvider
{
public:
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    void GatherActionsFor(const FUnitHandle& Target, ActionList& OutActions) override;

    void OpenContainer();
    void CloseContainer();

    /***************************************************************************************
    *  IContainer
    ****************************************************************************************/
    void GetContainerContents(std::vector<FUnitHandle>& OutContents) const override;
    
    bool CanAddToContainer(const FUnitHandle& InHandle) const override;
    void AddToContainer(FUnitHandle& InHandle) override;
    void RemoveFromContainer(FUnitHandle& InHandle) override;
    void GetItemPouchContainers(std::vector<IContainer*>& OutContainers) override;
    /***************************************************************************************/

protected:

    FAvalonTagQuery mFilter;
    std::vector<FUnitHandle> mContents;

    bool mIsOpen = false;

    /***************************************************************************************
    *  UI Helpers
    ****************************************************************************************/
public:
    // TODO:  All this, but like, better
    FUnitHandle GetContentsByIndex(int Index);
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

    std::vector<IContainer*> mOpenContainers;

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
    std::vector<FUnitHandle> mSlotContents;
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
    void GetContainerContents(std::vector<FUnitHandle>& OutContents) const override;
    
    bool CanAddToContainer(const FUnitHandle& Actor) const override;
    void AddToContainer(FUnitHandle& Actor) override;
    void RemoveFromContainer(FUnitHandle& Actor) override;
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



