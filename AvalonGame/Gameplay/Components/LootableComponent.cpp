/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "LootableComponent.h"
#include "../Actor/AvalonActor.h"

#include "ContainerComponent.h"
#include "../Actor/PlayerActor.h"
#include "../Actor/LevelActor.h"

#include <algorithm>
#include <iterator>

/***************************************************************************************
*  Lootable Component - I can be looted!  (Placed in Containers)
****************************************************************************************/
void Lootable::Load(FSaveContext& Context)
{
    // TODO:    mOnAdded
    // TODO:    mOnRemoved
}

void Lootable::Save(FSaveContext& Context)
{
    // TODO:    mOnAdded 
    // TODO:    mOnRemoved
}

void Lootable::GatherActionsFor(const FUnitHandle& Target, ActionList& OutActions)
{
    AvalonActor* Actor = Target.Get<AvalonActor>();
    std::vector<IContainer*> Containers;
    Actor->GatherContainers(Containers);

    // TODO:  Prevent dropping while travelling?
    // The "drop" option!
    AvalonActor* OuterActor = GetActorOwner();
    if (OuterActor->IsOwnedBy(PlayerActor::mPlayer->GetSelfHandle()))
    {
        if (IContainer* FloorContainer = LevelActor::mCurrentLevel->GetFloorContainer())
        {
            Containers.push_back(FloorContainer);
        }
    }

    for (IContainer* Container : Containers)
    {
        AvalonActor* ContainerActor = Container->GetActorOwner();
        if (ContainerActor->IsOwnedBy(GetActorOwnerHandle()))
        {
            continue; //Can't put myself, inside myself
        }

        if (Container->CanAddToContainer(GetActorOwnerHandle()))
        {
            AvalonAction* NewMove = new AvalonAction();

            NewMove->mActionPrompt = GenerateMoveActionString(Target, Container);

            Effect_MoveLoot* Effect = NewMove->AddEffect<Effect_MoveLoot>();
            Effect->mDestination = Container;
            Effect->mLoot = this;

            NewMove->mContext.mSource = GetActorOwnerHandle();
            NewMove->mContext.mTarget = Target;

            OutActions.push_back(NewMove);
        }
    }
}

void Lootable::OnAdded(IContainer* Container)
{
    mIsWithin = Container;

    // Run "On Added" actions!
    mOnAdded.mContext.mSource = GetActorOwnerHandle();                 // Lootable Actor is the Source
    mOnAdded.mContext.mTarget = Container->GetActorOwnerHandle();      // Container Actor is the Target
    mOnAdded.Execute();
}

void Lootable::OnRemoved(IContainer* Container)
{
    mIsWithin = nullptr;

    // Run "On Removed" actions!
    mOnRemoved.mContext.mSource = GetActorOwnerHandle();               // Lootable Actor is the Source
    mOnRemoved.mContext.mTarget = Container->GetActorOwnerHandle();    // Container Actor is the Target
    mOnRemoved.Execute();
}

std::string Lootable::GenerateMoveActionString( const FUnitHandle& Looter
                                              , IContainer* Destination)
{
    std::string RetString;
    std::string MyName = GetActorOwner()->mDisplayName;
    std::string ContainerName = Destination->GetActorOwner()->mDisplayName;

    if (Equipment* EquipmentContainer = dynamic_cast<Equipment*>(Destination))
    {
        if (Equipable* EquipableItem = dynamic_cast<Equipable*>(this))
        {
            if (EquipableItem->mSlotID == "Fastener")
            {
                RetString = "Fasten " + MyName + " to " + ContainerName;
            }
            else
            {
                RetString = "Equip " + MyName;
            }
        }
    }
    else
    {
        if (Destination == LevelActor::mCurrentLevel->GetFloorContainer())
        {
            RetString = "Drop " + MyName;
        }
        else
        {
            RetString = "Put " + MyName + " in " + ContainerName;
        }
    }

    return RetString;
}

void Lootable::MoveTo(IContainer* NewContainer)
{
    FUnitHandle Lootable = GetActorOwnerHandle();
    AvalonActor* LootableActor = Lootable.Get<AvalonActor>();

    // Store the chain of owners pre-move
    std::vector<FUnitHandle> OldOwners;
    Lootable.Get<AvalonActor>()->GatherOwners(OldOwners);
    std::sort(OldOwners.begin(), OldOwners.end(), [](auto& a, auto& b) { return a.Get() < b.Get(); });

    bool WasOwnedByPlayer = LootableActor->IsOwnedBy(PlayerActor::mPlayerHandle);

    // First! Remove it From where it is
    if (mIsWithin)
    {
        mIsWithin->RemoveFromContainer(Lootable);
    }

    //Next! Add it to where it is going
    NewContainer->AddToContainer(Lootable);

    // Get the new chain of owners post move
    std::vector<FUnitHandle> NewOwners;
    Lootable.Get<AvalonActor>()->GatherOwners(NewOwners);
    std::sort(NewOwners.begin(), NewOwners.end(), [](auto& a, auto& b) { return a.Get() < b.Get(); });

    // Do some vector math to tell the owners that lost
    // ownership that that happened.  And similarly
    // those who gained ownership that THAT happened.
    // AND this makes sure that parent owners where
    // ownership didn't change, don't get notified.  TREES!
    std::vector<FUnitHandle> RemovedOwners;
    std::set_difference(OldOwners.begin(), OldOwners.end(),
        NewOwners.begin(), NewOwners.end(),
        std::back_inserter(RemovedOwners),
        [](auto& a, auto& b) { return a.Get() < b.Get(); });

    for (FUnitHandle& Handle : RemovedOwners)
    {
        if (AvalonActor* Actor = Handle.Get<AvalonActor>())
        {
            Actor->mOnItemRemoved.BroadcastEvent(Lootable);
        }
    }

    std::vector<FUnitHandle> AddedOwners;
    std::set_difference(NewOwners.begin(), NewOwners.end(),
        OldOwners.begin(), OldOwners.end(),
        std::back_inserter(AddedOwners),
        [](auto& a, auto& b) { return a.Get() < b.Get(); });

    for (FUnitHandle& Handle : AddedOwners)
    {
        if (AvalonActor* Actor = Handle.Get<AvalonActor>())
        {
            Actor->mOnItemAdded.BroadcastEvent(Lootable);
        }
    }
}

/***************************************************************************************
*  Lootable Actions
****************************************************************************************/
void Effect_MoveLoot::ExecuteEffect(FEffectContext& Context)
{
    mLoot->MoveTo(mDestination);
}

/***************************************************************************************
*  Equipable Component - I can be equipped!
****************************************************************************************/
void Equipable::Load(FSaveContext& Context)
{
    Lootable::Load(Context);
    Context.Load("Slot", mSlotID);
}

void Equipable::Save(FSaveContext& Context)
{
    Lootable::Save(Context);
    Context.Save("Slot", mSlotID);
}

void Equipable::OnAdded(IContainer* Container)
{
    Lootable::OnAdded(Container);

    // Run "On Equipped" actions!
    mOnEquipped.mContext.mSource = GetActorOwnerHandle();              // Equipable Actor is the Source
    mOnEquipped.mContext.mTarget = Container->GetActorOwnerHandle();   // Equip Slot Actor is the Target
    mOnEquipped.Execute();
}

void Equipable::OnRemoved(IContainer* Container)
{
    Lootable::OnRemoved(Container);

    // Run "On Equipped" actions!
    mOnUnequipped.mContext.mSource = GetActorOwnerHandle();              // Equipable Actor is the Source
    mOnUnequipped.mContext.mTarget = Container->GetActorOwnerHandle();   // Equip Slot Actor is the Target
    mOnUnequipped.Execute();
}
