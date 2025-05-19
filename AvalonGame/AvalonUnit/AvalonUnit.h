/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include <vector>
#include <list>

class IAvalonUnit;
class AvalonMemory;

struct FUnitHandle
{
public:
    bool IsValid() const;
    void Reset();
    IAvalonUnit* Get() const;

    template <class T>
    T* Get() const
    {
        return (T*)(Get());
    }

    bool operator==(const FUnitHandle& rhs) const;
    bool operator!=(const FUnitHandle& rhs) const;

private:
    int mSlotID = -1;
    int mSlotGeneration = -1;

    friend class AvalonMemory;
};

struct FUnitSlot
{
    IAvalonUnit* mUnit = nullptr;
    int mSlotGeneration = -1;
};

// Classes allocated through this system aught to have
// virtualized destructors! (or put destruction logic in OnDestroyed)
class IAvalonUnit
{
public:

    // Returns true if not marked for destruction
    bool IsValid() const { return mManagedHandle.IsValid(); }
    FUnitHandle GetSelfHandle() const { return mManagedHandle; }

    virtual void OnCreated() {}
    virtual void OnDestroyed() {}

private:
    FUnitHandle mManagedHandle;

    friend class AvalonMemory;
};

class AvalonMemory
{
public:
    void TickGarbageCollection(float DeltaSeconds);

    template <class T>
    static FUnitHandle NewUnit()
    {
        T* Unit = new T;

        if (IAvalonUnit* AvalonUnit = static_cast<IAvalonUnit*>(Unit))
        {
            AvalonUnit->OnCreated();
        }

        return AvalonMemory::Get().AllocateUnit(Unit);
    }

    static void DestroyUnit(IAvalonUnit* Unit);

    IAvalonUnit* GetUnitFromHandle(const FUnitHandle& Handle);

private:
    FUnitHandle AllocateUnit(IAvalonUnit* Unit);

    void MarkForGarbageCollection(FUnitHandle& Handle);
    void DoGarbageCollection();

    std::vector<FUnitSlot*> mAllocatedUnits;
    std::list<int> mFreeSlots;
    std::list<int> mGarbageSlots;

    bool mForceGarbageCollection = false;
    float mGarbageCollectionDelay = 30.f;
    float mTimeSinceLastGarbageCollection = 0.f;

    //////////////////////////////////////////////////////////////////////////
    //  Singleton Implementation
public:
    static AvalonMemory& Get()
    {
        static AvalonMemory Manager;
        return Manager;
    }

    AvalonMemory(AvalonMemory const&) = delete;
    void operator=(AvalonMemory const&) = delete;

private:
    AvalonMemory() {}
    ~AvalonMemory() {}
    //////////////////////////////////////////////////////////////////////////
};
