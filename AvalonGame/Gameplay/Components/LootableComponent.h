/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../Actor/ActorTypes.h"
#include "../Utility/AvalonTags.h"
#include "../Actions/AvalonAction.h"

#include <vector>

class IContainer;
/***************************************************************************************
*  Lootable Component - I can be looted!  (Put in Item Containers)
****************************************************************************************/
class Lootable: public IActorComponent, public IActionProvider
{
public:
    virtual void Load(FSaveContext& Context) override;
    virtual void Save(FSaveContext& Context) override;

    void GatherActionsFor(const FUnitHandle& Target, ActionList& OutActions) override;

    virtual void OnAdded(IContainer* Container);
    virtual void OnRemoved(IContainer* Container);

    std::string GenerateMoveActionString( const FUnitHandle& Looter
                                        , IContainer* Destination);

    void MoveTo(IContainer* NewContainer);
private:
    AvalonAction mOnAdded;
    AvalonAction mOnRemoved;

protected:
    IContainer* mIsWithin = nullptr;
};

/***************************************************************************************
*  Lootable Actions
****************************************************************************************/
struct Effect_MoveLoot : public IAvalonEffect
{
    virtual void ExecuteEffect(FEffectContext& Context) override;
    class Lootable* mLoot = nullptr;
    class IContainer* mDestination = nullptr;
};

/***************************************************************************************
*  Equipable Component - I can be equipped!
****************************************************************************************/
class Equipable : public Lootable
{
public:
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    void OnAdded(IContainer* Container) override;
    void OnRemoved(IContainer* Container) override;

    // Keeping it simple
    std::string mSlotID;

private:
    AvalonAction mOnEquipped;
    AvalonAction mOnUnequipped;
};
