/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "LootableComponent.h"
#include "../Actor/AvalonActor.h"

#include "ContainerComponent.h"
#include "../Actor/PlayerActor.h"
#include "../Actor/LevelActor.h"

#include "../AvalonGameState.h"

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

void Lootable::GatherActionsFor(const AvalonActor* Target, ActionList& OutActions)
{
    ContainerList Containers;
    Target->GatherContainers(Containers);

    // TODO:  Prevent dropping while travelling?
    // The "drop" option!
    AvalonActor* OuterActor = GetActorOwner();
    LevelActor* CurrentLevel = AvalonGameState::GetCurrentLevel();
    AvalonActor* Player = AvalonGameState::GetPlayerActor();
    if (OuterActor->IsOwnedBy(Player))
    {
        if (IContainer* FloorContainer = CurrentLevel->GetFloorContainer())
        {
            Containers.push_back(FloorContainer);
        }
    }

    for (IContainer* Container : Containers)
    {
        AvalonActor* ContainerActor = Container->GetActorOwner();
        if (ContainerActor->IsOwnedBy(OuterActor))
        {
            continue; //Can't put myself, inside myself
        }
        if (Container->CanAddToContainer(OuterActor))
        {
            AvalonAction* NewMove = new AvalonAction();

            NewMove->mActionPrompt = GenerateMoveActionString(Target, Container);

            Effect_MoveLoot* Effect = NewMove->AddEffect<Effect_MoveLoot>();
            Effect->mDestination = Container;
            Effect->mLoot = this;

            HardUnitRef SourceRef = OuterActor->GetSelfRef();
            HardUnitRef TargetRef = Target->GetSelfRef();

            NewMove->mContext.mSource = SourceRef;
            NewMove->mContext.mTarget = TargetRef;

            OutActions.push_back(NewMove);
        }
    }
}

void Lootable::OnAdded(IContainer* Container)
{
    mIsWithin = Container;

    HardUnitRef SourceRef = GetActorOwner()->GetSelfRef();
    HardUnitRef TargetRef = Container->GetActorOwner()->GetSelfRef();

    // Run "On Added" actions!
    mOnAdded.mContext.mSource = SourceRef;  // Lootable Actor is the Source
    mOnAdded.mContext.mTarget = TargetRef;  // Container Actor is the Target
    mOnAdded.Execute();
}

void Lootable::OnRemoved(IContainer* Container)
{
    mIsWithin = nullptr;

    HardUnitRef SourceRef = GetActorOwner()->GetSelfRef();
    HardUnitRef TargetRef = Container->GetActorOwner()->GetSelfRef();

    // Run "On Removed" actions!
    mOnRemoved.mContext.mSource = SourceRef;    // Lootable Actor is the Source
    mOnRemoved.mContext.mTarget = TargetRef;    // Container Actor is the Target
    mOnRemoved.Execute();
}

std::string Lootable::GenerateMoveActionString( const AvalonActor* Looter
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
        LevelActor* CurrentLevel = AvalonGameState::GetCurrentLevel();
        if (Destination == CurrentLevel->GetFloorContainer())
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
    AvalonActor* LootableActor = GetActorOwner();

    // Store the chain of owners pre-move
    ActorList OldOwners;
    LootableActor->GatherOwners(OldOwners);
    std::sort(OldOwners.begin(), OldOwners.end(), [](auto& a, auto& b) { return a < b; });

    AvalonActor* PlayerActor = AvalonGameState::GetPlayerActor();
    bool WasOwnedByPlayer = LootableActor->IsOwnedBy(PlayerActor);

    // First! Remove it From where it is
    if (mIsWithin)
    {
        HardUnitRef LootableActorRef = LootableActor->GetSelfRef();
        mIsWithin->RemoveFromContainer(LootableActorRef);
    }

    //Next! Add it to where it is going
    NewContainer->AddToContainer(LootableActor);

    // Get the new chain of owners post move
    ActorList NewOwners;
    LootableActor->GatherOwners(NewOwners);
    std::sort(NewOwners.begin(), NewOwners.end(), [](auto& a, auto& b) { return a < b; });

    // Do some vector math to tell the owners that lost
    // ownership that that happened.  And similarly
    // those who gained ownership that THAT happened.
    // AND this makes sure that parent owners where
    // ownership didn't change, don't get notified.  TREES!
    ActorList RemovedOwners;
    std::set_difference(OldOwners.begin(), OldOwners.end(),
                        NewOwners.begin(), NewOwners.end(),
        std::back_inserter(RemovedOwners),
        [](auto& a, auto& b) { return a < b; });

    for (auto& Actor : RemovedOwners)
    {
        if (Actor != nullptr)
        {
            Actor->mOnItemRemoved.BroadcastEvent(LootableActor);
        }
    }

    ActorList AddedOwners;
    std::set_difference( NewOwners.begin(), NewOwners.end(),
                         OldOwners.begin(), OldOwners.end(),
        std::back_inserter(AddedOwners),
        [](auto& a, auto& b) { return a < b; });

    for (auto& Actor : AddedOwners)
    {
        if (Actor != nullptr)
        {
            Actor->mOnItemAdded.BroadcastEvent(LootableActor);
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

    HardUnitRef SourceRef = GetActorOwner()->GetSelfRef();
    HardUnitRef TargetRef = Container->GetActorOwner()->GetSelfRef();

    // Run "On Equipped" actions!
    mOnEquipped.mContext.mSource = SourceRef;   // Equipable Actor is the Source
    mOnEquipped.mContext.mTarget = TargetRef;   // Equip Slot Actor is the Target
    mOnEquipped.Execute();
}

void Equipable::OnRemoved(IContainer* Container)
{
    Lootable::OnRemoved(Container);

    HardUnitRef SourceRef = GetActorOwner()->GetSelfRef();
    HardUnitRef TargetRef = Container->GetActorOwner()->GetSelfRef();

    // Run "On Equipped" actions!
    mOnUnequipped.mContext.mSource = SourceRef;   // Equipable Actor is the Source
    mOnUnequipped.mContext.mTarget = TargetRef;   // Equip Slot Actor is the Target
    mOnUnequipped.Execute();
}
