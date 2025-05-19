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
void IContainer::GetContainersWithin(std::vector<IContainer*>& OutContainers)
{
    std::vector<FUnitHandle> Contents;
    GetContainerContents(Contents);
    for (FUnitHandle& Handle : Contents)
    {
        if (AvalonActor* Actor = Handle.Get<AvalonActor>())
        {
            Actor->GatherContainers(OutContainers);
        }
    }
}

bool IContainer::IsInContainer(const FUnitHandle& ActorHandle) const
{
    std::vector<FUnitHandle> Contents;
    GetContainerContents(Contents);

    for (FUnitHandle& CheckHandle : Contents)
    {
        if (CheckHandle == ActorHandle)
        {
            return true;
        }
    }

    return false;
}

bool IContainer::CanAddToContainer(const FUnitHandle& ActorHandle) const
{
    const FUnitHandle& MyActorHandle = GetActorOwnerHandleConst();
    return (ActorHandle != MyActorHandle && IsInContainer(ActorHandle) == false);
}

void IContainer::AddToContainer(FUnitHandle& ToAddHandle)
{
    if (ToAddHandle.IsValid())
    {
        AvalonActor* ToAddActor = ToAddHandle.Get<AvalonActor>();
        ToAddActor->SetOwner(GetActorOwnerHandle());

        if (Lootable* Item = ToAddActor->GetComponent<Lootable>())
        {
            Item->OnAdded(this);
        }
    }
}

void IContainer::RemoveFromContainer(FUnitHandle& ToRemoveHandle)
{
    if (ToRemoveHandle.IsValid())
    {
        AvalonActor* ToRemoveActor = ToRemoveHandle.Get<AvalonActor>();
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
    LevelActor* Level = GetActorOwnerHandle().Get<LevelActor>();
    Level->SetLevelContainer(this);

    auto LoadActors_Lamdba = [&](FSaveContext& InContext)
    {
        FUnitHandle Handle = LoadChildActor(InContext);
        AddToContainer(Handle);
    };

    Context.LoadChildren(LoadActors_Lamdba);
}

void LevelContainer::Save(FSaveContext& Context)
{
    for (FUnitHandle& Handle : mLevelActors)
    {
        if (AvalonActor* Actor = Handle.Get<AvalonActor>())
        {
            ISaveable::Save(Actor, Context);
        }
    }
}

void LevelContainer::Tick(float DeltaSeconds)
{
    for (FUnitHandle& ActorHandle : mLevelActors)
    {
        if (AvalonActor* Actor = ActorHandle.Get<AvalonActor>())
        {
            Actor->Tick(DeltaSeconds);
        }
    }
}

void LevelContainer::AdvanceTime(long DeltaHours)
{
    for (FUnitHandle& ActorHandle : mLevelActors)
    {
        if (AvalonActor* Actor = ActorHandle.Get<AvalonActor>())
        {
            Actor->AdvanceTime(DeltaHours);
        }
    }
}

void LevelContainer::GetContainerContents(std::vector<FUnitHandle>& OutContents) const
{
    OutContents = mLevelActors;
}

bool LevelContainer::CanAddToContainer(const FUnitHandle& Handle) const
{
    return IContainer::CanAddToContainer(Handle);
}

void LevelContainer::AddToContainer(FUnitHandle& Handle)
{
    IContainer::AddToContainer(Handle);
    mLevelActors.push_back(Handle);
}

void LevelContainer::RemoveFromContainer(FUnitHandle& Handle)
{
    auto position = std::find(mLevelActors.begin(), mLevelActors.end(), Handle);
    if (position != mLevelActors.end())
    {
        mLevelActors.erase(position);
    }

    IContainer::RemoveFromContainer(Handle);
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
        FUnitHandle Handle = LoadChildActor(InContext);
        AddToContainer(Handle);
    };

    mFilter.Load(Context);
    Context.LoadFromWrapper("Contents", LoadItem_Lamdba);
}

void ItemContainer::Save(FSaveContext& Context)
{
    mFilter.Save(Context);

    // Because Invalid Handles Save UGH
    std::vector<AvalonActor*> ActorList;
    for (FUnitHandle& Handle : mContents)
    {
        if (AvalonActor* Actor = Handle.Get<AvalonActor>())
        {
            ActorList.push_back(Actor);
        }
    }

    Context.SaveUnderWrapper("Contents", ActorList);
}

void ItemContainer::GatherActionsFor(const FUnitHandle& Target, ActionList& OutActions)
{
    // TODO:  Don't allow the LOOTER to act on this if another LOOTER is
    AvalonAction* ContainerAction = new AvalonAction();
    if (mIsOpen)
    {
        ContainerAction->mActionPrompt = "Close " + GetActorOwnerHandle().Get<AvalonActor>()->mDisplayName;
        Effect_CloseContainer* Effect = ContainerAction->AddEffect<Effect_CloseContainer>();
        Effect->mContainer = this;
    }
    else
    {
        ContainerAction->mActionPrompt = "Open " + GetActorOwnerHandle().Get<AvalonActor>()->mDisplayName;
        Effect_OpenContainer* Effect = ContainerAction->AddEffect<Effect_OpenContainer>();
        Effect->mContainer = this;
    }

    ContainerAction->mContext.mSource = GetActorOwnerHandle();
    ContainerAction->mContext.mTarget = Target;

    OutActions.push_back(ContainerAction);
}

void ItemContainer::OpenContainer()
{
    mIsOpen = true;
}

void ItemContainer::CloseContainer()
{
    mIsOpen = false;
}

void ItemContainer::AddToContainer(FUnitHandle& Handle)
{
    mContents.push_back(Handle);
    IContainer::AddToContainer(Handle);
}

void ItemContainer::RemoveFromContainer(FUnitHandle& Handle)
{

    std::vector<FUnitHandle>::iterator position = std::find( mContents.begin()
                                                            , mContents.end(), Handle);
    if (position != mContents.end())
    {
        mContents.erase(position);
    }      
    IContainer::RemoveFromContainer(Handle);
}

bool ItemContainer::CanAddToContainer(const FUnitHandle& Handle) const
{
    if (AvalonActor* Actor = Handle.Get<AvalonActor>())
    {
        if (!mFilter.PassesQuery(Actor->mTags))
        {
            return false;
        }

        // TODO: Capacity Check
    }

    return IContainer::CanAddToContainer(Handle);
}

void ItemContainer::GetContainerContents(std::vector<FUnitHandle>& OutContents) const
{
    OutContents = mContents;
}

void ItemContainer::GetItemPouchContainers(std::vector<IContainer*>& OutContainers)
{
    OutContainers.push_back(this);
}

/***************************************************************************************
*  Container UI Helpers
****************************************************************************************/
FUnitHandle ItemContainer::GetContentsByIndex(int Index)
{
    return mContents[Index];
}

std::string ItemContainer::GetDisplayName()
{
    return GetActorOwnerHandle().Get<AvalonActor>()->mDisplayName;
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
    LooterComponent* Looter = Context.mTarget.Get<AvalonActor>()->GetComponent<LooterComponent>();
    Looter->OpenContainer(mContainer);
}

void Effect_CloseContainer::ExecuteEffect(FEffectContext& Context)
{
    LooterComponent* Looter = Context.mTarget.Get<AvalonActor>()->GetComponent<LooterComponent>();
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

        FUnitHandle Handle = Parent->LoadChildActor(InContext);
        mSlotContents.push_back(Handle);
    };

    mID = Context.GetSaveID();

    Context.LoadChildren(LoadEquipment_Lamdba);
    Context.Load("Capacity", mMaxCapacity);
}

void FEquipSlot::Save(FSaveContext& Context)
{
    // Because Invalid Handles Save UGH
    std::vector<AvalonActor*> ActorList;
    for (FUnitHandle& Handle : mSlotContents)
    {
        if (AvalonActor* Actor = Handle.Get<AvalonActor>())
        {
            ActorList.push_back(Actor);
        }
    }

    Context.SaveUnderWrapper(mID.c_str(), ActorList);

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

bool Equipment::CanAddToContainer(const FUnitHandle& Handle) const
{
    if (IContainer::CanAddToContainer(Handle) == false)
    {
        return false;
    }

    Equipable* Item = Handle.Get<AvalonActor>()->GetComponent<Equipable>();
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

void Equipment::AddToContainer(FUnitHandle& Handle)
{
    Equipable* Item = Handle.Get<AvalonActor>()->GetComponent<Equipable>();
    if (Item)
    {
        if (FEquipSlot* Slot = GetSlotByID(Item->mSlotID))
        {
            Slot->mSlotContents.push_back(Handle);
        }
    }
    IContainer::AddToContainer(Handle);
}

void Equipment::RemoveFromContainer(FUnitHandle& Handle)
{
    Equipable* Item = Handle.Get<AvalonActor>()->GetComponent<Equipable>();
    if (Item)
    {
        FEquipSlot* Slot = GetSlotByID(Item->mSlotID);

        auto It = std::find( Slot->mSlotContents.begin()
                           , Slot->mSlotContents.end(), Handle);
        if (It != Slot->mSlotContents.end())
        {
            Slot->mSlotContents.erase(It);
        }
    }
    IContainer::RemoveFromContainer(Handle);
}

void Equipment::GetContainerContents(std::vector<FUnitHandle>& OutContents) const
{
    for (auto& elem : mEquipment)
    {
        for (auto& subElem : elem.mSlotContents)
        {
            OutContents.push_back(subElem);
        }
    }
}

void Equipment::GetItemPouchContainers(std::vector<IContainer*>& OutContainers)
{
    for (auto& elem : mEquipment)
    {
        for (auto& Handle : elem.mSlotContents)
        {
            if (AvalonActor* Actor = Handle.Get<AvalonActor>())
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
