/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "ContainerComponent.h"
#include "LootableComponent.h"

#include "../Actor/AvalonActor.h"
#include "../Actor/LevelActor.h"

/***************************************************************************************
*  IContainer
****************************************************************************************/
void IContainer::GetContainersWithin(ContainerList& OutContainers)
{
    HardRefList Contents;
    GetContainerContents(Contents);
    for (auto& ActorRef : Contents)
    {
        AvalonActor* Actor = Get<AvalonActor>(ActorRef);
        if (Actor != nullptr)
        {
            Actor->GatherContainers(OutContainers);
        }
    }
}

bool IContainer::IsInContainer(const AvalonActor* InActor) const
{
    if (InActor != nullptr)
    {
        HardRefList Contents;
        GetContainerContents(Contents);

        for (auto& CheckActorRef : Contents)
        {
            AvalonActor* CheckActor = Get<AvalonActor>(CheckActorRef);
            if (CheckActor == InActor)
            {
                return true;
            }
        }
    }

    return false;
}

bool IContainer::CanAddToContainer(const AvalonActor* Actor) const
{
    return (IsInContainer(Actor) == false);
}

void IContainer::AddToContainer(AvalonActor* ToAdd)
{
    if (ToAdd != nullptr)
    {
        AvalonActor* ContainerOwner = GetActorOwner();
        HardUnitRef ContainerOwnerRef = ContainerOwner->GetSelfRef();
        ToAdd->SetOwner(ContainerOwnerRef);

        if (Lootable* Item = ToAdd->GetComponent<Lootable>())
        {
            Item->OnAdded(this);
        }
    }
}

void IContainer::RemoveFromContainer(HardUnitRef& ToRemove)
{
    AvalonActor* ToRemoveActor = Get<AvalonActor>(ToRemove);
    if (ToRemoveActor != nullptr)
    {
        ToRemoveActor->ResetOwner();

        if (Lootable* Loot = ToRemoveActor->GetComponent<Lootable>())
        {
            Loot->OnRemoved(this);
        }
    }
}

/***************************************************************************************
*  Level Container
****************************************************************************************/
void LevelContainer::Load(FSaveContext& Context)
{
    AvalonActor* OwnerActor = GetActorOwner();
    LevelActor* Level = OwnerActor ? OwnerActor->GetLevel() : nullptr;
    Level->SetLevelContainer(this);

    auto LoadActors_Lamdba = [&](FSaveContext& InContext)
    {
        HardUnitRef ActorRef;
        LoadChildActor(ActorRef, InContext);
        AddToContainer(AvalonActor::Get<AvalonActor>(ActorRef));
    };

    Context.LoadChildren(LoadActors_Lamdba);
}

void LevelContainer::Save(FSaveContext& Context)
{
    for (HardUnitRef& ActorRef : mLevelActors)
    {
        if (ActorRef != nullptr)
        {
            AvalonActor* Actor = Get<AvalonActor>(ActorRef);
            ISaveable::Save(Actor, Context);
        }
    }
}

void LevelContainer::Tick(float DeltaSeconds)
{
    for (HardUnitRef& ActorRef : mLevelActors)
    {
        if (ActorRef != nullptr)
        {
            AvalonActor* Actor = IAvalonUnit::Get<AvalonActor>(ActorRef);
            Actor->Tick(DeltaSeconds);
        }
    }
}

void LevelContainer::AdvanceTime(long DeltaHours)
{
    for (HardUnitRef& ActorRef : mLevelActors)
    {
        if (ActorRef != nullptr)
        {
            AvalonActor* Actor = IAvalonUnit::Get<AvalonActor>(ActorRef);
            Actor->AdvanceTime(DeltaHours);
        }
    }
}

void LevelContainer::GetContainerContents(std::vector<HardUnitRef>& OutContents) const
{
    OutContents = mLevelActors;
}

bool LevelContainer::CanAddToContainer(const AvalonActor* Actor) const
{
    return IContainer::CanAddToContainer(Actor);
}

void LevelContainer::AddToContainer(AvalonActor* Actor)
{
    IContainer::AddToContainer(Actor);

    HardUnitRef ActorRef = Actor->GetSelfRef();
    mLevelActors.push_back(ActorRef);
}

void LevelContainer::RemoveFromContainer(HardUnitRef& ToRemove)
{
    auto position = std::find(mLevelActors.begin(), mLevelActors.end(), ToRemove);
    if (position != mLevelActors.end())
    {
        mLevelActors.erase(position);
    }

    IContainer::RemoveFromContainer(ToRemove);
}

/***************************************************************************************
*  Item Container - Like a bag! Has slots!
****************************************************************************************/
void ItemContainer::Load(FSaveContext& Context)
{
    // Expected Layout:
    /*
        <ItemContainer>
            <TagFilterQuery />
            <Contents>
                <Apple ... />
                <Apple ... />
            </Contents>
        </ItemContainer>
    */

    auto LoadItem_Lamdba = [&](FSaveContext& InContext)
    { 
        HardUnitRef ActorRef;
        LoadChildActor(ActorRef, InContext);
        AddToContainer(AvalonActor::Get<AvalonActor>(ActorRef));
    };

    mFilter.Load(Context);
    Context.LoadFromWrapper("Contents", LoadItem_Lamdba);
}

void ItemContainer::Save(FSaveContext& Context)
{
    mFilter.Save(Context);

    // Because Invalid Handles Save UGH
    std::vector<AvalonActor*> ActorList;
    for (HardUnitRef& ActorRef : mContents)
    {
        if (ActorRef != nullptr)
        {
            AvalonActor* Actor = Get<AvalonActor>(ActorRef);
            ActorList.push_back(Actor);
        }
    }

    Context.SaveUnderWrapper("Contents", ActorList);
}

void ItemContainer::GatherActionsFor(const AvalonActor* Target, ActionList& OutActions)
{
    AvalonActor* OwnerActor = GetActorOwner();
    HardUnitRef OwnerActorRef = OwnerActor->GetSelfRef();

    // TODO:  Don't allow the LOOTER to act on this if another LOOTER is
    HardUnitRef ContainerActionRef;
    AvalonAction* ContainerAction = AvalonAction::NewAction(ContainerActionRef);
    if (mIsOpen)
    {
        ContainerAction->mActionPrompt = "Close " + OwnerActor->mDisplayName;
        Effect_CloseContainer* Effect = ContainerAction->AddEffect<Effect_CloseContainer>();
        Effect->mContainer = this;
    }
    else
    {
        ContainerAction->mActionPrompt = "Open " + OwnerActor->mDisplayName;
        Effect_OpenContainer* Effect = ContainerAction->AddEffect<Effect_OpenContainer>();
        Effect->mContainer = this;
    }

    ContainerAction->mContext.mSource = OwnerActorRef;
    ContainerAction->mContext.mTarget = Target->GetSelfRef();

    OutActions.push_back(ContainerActionRef);
}

void ItemContainer::OpenContainer()
{
    mIsOpen = true;
}

void ItemContainer::CloseContainer()
{
    mIsOpen = false;
}

void ItemContainer::AddToContainer(AvalonActor* Actor)
{
    HardUnitRef ActorRef = Actor->GetSelfRef();
    mContents.push_back(ActorRef);

    IContainer::AddToContainer(Actor);
}

void ItemContainer::RemoveFromContainer(HardUnitRef& ToRemove)
{
    std::vector<HardUnitRef>::iterator position = std::find( mContents.begin()
                                                            , mContents.end(), ToRemove);
    if (position != mContents.end())
    {
        mContents.erase(position);
    }

    IContainer::RemoveFromContainer(ToRemove);
}

bool ItemContainer::CanAddToContainer(const AvalonActor* Actor) const
{
    if (Actor != nullptr)
    {
        if (!mFilter.PassesQuery(Actor->mTags))
        {
            return false;
        }

        // TODO: Capacity Check
    }

    return IContainer::CanAddToContainer(Actor);
}

void ItemContainer::GetContainerContents(HardRefList& OutContents) const
{
    OutContents = mContents;
}

void ItemContainer::GetItemPouchContainers(ContainerList& OutContainers)
{
    OutContainers.push_back(this);
}

/***************************************************************************************
*  Container UI Helpers
****************************************************************************************/
HardUnitRef ItemContainer::GetContentsByIndex(int Index)
{
    return mContents[Index];
}

std::string ItemContainer::GetDisplayName()
{
    return GetActorOwner()->mDisplayName;
}

int ItemContainer::GetNumItems()
{
    return mContents.size();
}

int ItemContainer::GetMaxSlots()
{
    return 10;
}

/***************************************************************************************
*  Container Effects
****************************************************************************************/
void Effect_OpenContainer::ExecuteEffect(FEffectContext& Context)
{
    AvalonActor* LooterActor = AvalonActor::Get<AvalonActor>(Context.mTarget);
    LooterComponent* Looter = LooterActor->GetComponent<LooterComponent>();
    Looter->OpenContainer(mContainer);
}

void Effect_CloseContainer::ExecuteEffect(FEffectContext& Context)
{
    AvalonActor* LooterActor = AvalonActor::Get<AvalonActor>(Context.mTarget);
    LooterComponent* Looter = LooterActor->GetComponent<LooterComponent>();
    Looter->CloseContainer(mContainer);
}

/***************************************************************************************
*  Looter Component - I can open Containers and move Lootables around!
****************************************************************************************/
void LooterComponent::OpenContainer(IContainer* Container)
{
    mOpenContainers.push_back(Container);

    mOnContainerOpened.BroadcastEvent(Container);
}

void LooterComponent::CloseContainer(IContainer* Container)
{
    std::vector<IContainer*>::iterator position = std::find( mOpenContainers.begin()
                                                           , mOpenContainers.end(), Container);
    if (position != mOpenContainers.end())
    {
        mOpenContainers.erase(position);
    }

    mOnContainerOpened.BroadcastEvent(Container);
}

/***************************************************************************************
*  Equipment Component - I can equip things!
****************************************************************************************/
void FEquipSlot::Load(FSaveContext& Context, IActorComponent* Parent)
{
    // Expected Layout:
    /*
        <Fastener Capacity="3">
            <LeatherPouch ... />
            <LeatherPouch ... />
            <LeatherPouch ... />
        </Fastener>
    */

    auto LoadEquipment_Lamdba = [&](FSaveContext& InContext)
    {
        //FSaveContext ChildContext;
        //InContext.GetChild(ChildContext);

        HardUnitRef ActorRef;
        Parent->LoadChildActor(ActorRef, InContext);
        mSlotContents.push_back(ActorRef);
    };

    mID = Context.GetSaveID();

    Context.LoadChildren(LoadEquipment_Lamdba);
    Context.Load("Capacity", mMaxCapacity);
}

void FEquipSlot::Save(FSaveContext& Context)
{
    ActorList Actors;
    for (HardUnitRef& ActorRef : mSlotContents)
    {
        if (AvalonActor* Actor = AvalonActor::Get<AvalonActor>(ActorRef))
        {
            Actors.push_back(Actor);
        }
    }

    Context.SaveUnderWrapper(mID.c_str(), Actors);

    FSaveContext ChildContext;
    Context.GetChild(ChildContext, mID.c_str());
    ChildContext.Save("Capacity", mMaxCapacity);
}

void Equipment::Load(FSaveContext& Context)
{
    // Expected Layout:
    /*
        <Equipment>
			<Undershirt>
                <LinenShirt ... />
            </Undershirt>
			<Torso/>
			<Cloak/>
			<Hands/>
			<Waist/>
			<Legs/>
			<Feet/>
		</Equipment>
    */


    auto LoadSlots_Lamdba = [&](FSaveContext& InContext)
    {
        FEquipSlot NewSlot;
        NewSlot.Load(InContext, this);

        mEquipment.push_back(NewSlot);
    };

    Context.LoadChildren(LoadSlots_Lamdba);
}

void Equipment::Save(FSaveContext& Context)
{
    for (auto& elem : mEquipment)
    {
        elem.Save(Context);
    }
}

bool Equipment::CanAddToContainer(const AvalonActor* Actor) const
{
    if (IContainer::CanAddToContainer(Actor) == false)
    {
        return false;
    }

    Equipable* Item = Actor->GetComponent<Equipable>();
    if (Item)
    {
        const FEquipSlot* Slot = GetSlotByID(Item->mSlotID);
        if (Slot != nullptr)
        {
            if (Slot->mSlotContents.size() < Slot->mMaxCapacity)
            {
                return true;
            }
        }
    }

    return false;
}

void Equipment::AddToContainer(AvalonActor* Actor)
{
    Equipable* Item = Actor->GetComponent<Equipable>();
    if (Item)
    {
        if (FEquipSlot* Slot = GetSlotByID(Item->mSlotID))
        {
            HardUnitRef ActorRef = Actor->GetSelfRef();
            Slot->mSlotContents.push_back(ActorRef);
        }
    }
    IContainer::AddToContainer(Actor);
}

void Equipment::RemoveFromContainer(HardUnitRef& ActorRef)
{
    AvalonActor* Actor = Get<AvalonActor>(ActorRef);
    Equipable* Item = Actor->GetComponent<Equipable>();
    if (Item)
    {
        FEquipSlot* Slot = GetSlotByID(Item->mSlotID);

        auto It = std::find( Slot->mSlotContents.begin()
                           , Slot->mSlotContents.end(), ActorRef);
        if (It != Slot->mSlotContents.end())
        {
            Slot->mSlotContents.erase(It);
        }
    }
    IContainer::RemoveFromContainer(ActorRef);
}

void Equipment::GetContainerContents(HardRefList& OutContents) const
{
    for (auto& elem : mEquipment)
    {
        for (auto& subElem : elem.mSlotContents)
        {
            OutContents.push_back(subElem);
        }
    }
}

void Equipment::GetItemPouchContainers(ContainerList& OutContainers)
{
    for (auto& elem : mEquipment)
    {
        for (HardUnitRef& ActorRef : elem.mSlotContents)
        {
            AvalonActor* Actor = Get<AvalonActor>(ActorRef);
            if (Actor != nullptr)
            {
                Actor->GetItemPouchContainers(OutContainers);
            }
        }
    }
}

const FEquipSlot* Equipment::GetSlotByID(std::string ID) const
{
    for (auto& elem : mEquipment)
    {
        if (elem.mID == ID)
        {
            return &elem;
        }
    }

    return nullptr;
}

FEquipSlot* Equipment::GetSlotByID(std::string ID)
{
    for (auto& elem : mEquipment)
    {
        if (elem.mID == ID)
        {
            return &elem;
        }
    }

    return nullptr;
}
