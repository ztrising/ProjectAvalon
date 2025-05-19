/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonUnit.h"
#include "../Gameplay/AvalonGameState.h"

bool FUnitHandle::IsValid() const
{
	return mSlotID != -1;
}

void FUnitHandle::Reset()
{
    mSlotID = -1;
    mSlotGeneration = -1;
}

IAvalonUnit* FUnitHandle::Get() const
{
    return AvalonMemory::Get().GetUnitFromHandle(*this);
}

bool FUnitHandle::operator==(const FUnitHandle& rhs) const
{
    return (mSlotID == rhs.mSlotID) && (mSlotGeneration == rhs.mSlotGeneration);
}

bool FUnitHandle::operator!=(const FUnitHandle& rhs) const
{
    return (mSlotID != rhs.mSlotID) || (mSlotGeneration != rhs.mSlotGeneration);
}

IAvalonUnit* AvalonMemory::GetUnitFromHandle(const FUnitHandle& Handle)
{
    if (Handle.mSlotID >= 0 && Handle.mSlotID < mAllocatedUnits.size())
    {
        FUnitSlot* Slot = mAllocatedUnits[Handle.mSlotID];
        if (Slot->mSlotGeneration == Handle.mSlotGeneration)
        {
            return Slot->mUnit;
        }
    }

    return nullptr;
}

FUnitHandle AvalonMemory::AllocateUnit(IAvalonUnit* Unit)
{
    FUnitHandle Handle;
    FUnitSlot* Slot = nullptr;

    if (mFreeSlots.size() > 0)
    {
        int SlotIndex = mFreeSlots.front();
        mFreeSlots.pop_front();
        Slot = mAllocatedUnits[SlotIndex];
        Handle.mSlotID = SlotIndex;
    }
    else
    {
        Slot = new FUnitSlot();
        Slot->mSlotGeneration++;

        mAllocatedUnits.push_back(Slot);
        Handle.mSlotID = mAllocatedUnits.size() - 1;
    }

    Handle.mSlotGeneration = Slot->mSlotGeneration;
    Slot->mUnit = Unit;
    Unit->mManagedHandle = Handle;

    return Handle;
}

/*static*/ void AvalonMemory::DestroyUnit(IAvalonUnit* Unit)
{
	if (Unit)
	{
		Unit->OnDestroyed();
        AvalonMemory::Get().MarkForGarbageCollection(Unit->mManagedHandle);
	}
}

void AvalonMemory::MarkForGarbageCollection(FUnitHandle& Handle)
{
	mGarbageSlots.push_back(Handle.mSlotID);
	Handle.Reset();
}

void AvalonMemory::TickGarbageCollection(float DeltaSeconds)
{
    mTimeSinceLastGarbageCollection += DeltaSeconds;
    bool ShouldGC =  mForceGarbageCollection 
                  || (mTimeSinceLastGarbageCollection > mGarbageCollectionDelay);
    if (ShouldGC)
    {
        DoGarbageCollection();
        mForceGarbageCollection = false;
        mTimeSinceLastGarbageCollection = 0.f;
    }
}

void AvalonMemory::DoGarbageCollection()
{
    while (mGarbageSlots.size() > 0)
    {
        int SlotID = mGarbageSlots.front();
        mGarbageSlots.pop_front();
        if (SlotID >= 0 && SlotID < mAllocatedUnits.size())
        {
			FUnitSlot* Slot = mAllocatedUnits[SlotID];
			Slot->mSlotGeneration++;

            delete Slot->mUnit;
			Slot->mUnit = nullptr;

			mFreeSlots.push_back(SlotID);
        }
    }

    // TODO : resize mAllocatedUnits conditionally?
}
