/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include <vector>
#include <list>
#include <memory>

class IAvalonUnit;
class AvalonMemory;

class IAvalonUnit;
typedef std::shared_ptr<IAvalonUnit> HardUnitRef;
typedef std::weak_ptr<IAvalonUnit> SoftUnitRef;

typedef std::vector<HardUnitRef> HardRefList;


// Classes allocated through this system aught to have
// virtualized destructors! (or put destruction logic in OnDestroyed)
class IAvalonUnit
{
public:

    bool operator==(const IAvalonUnit& rhs) const;
    bool operator!=(const IAvalonUnit& rhs) const;

    template <class T>
    static T* Get(HardUnitRef HardRef)
    {
        return (T*)(HardRef.get());
    }

    template <class T>
    static T* Get(SoftUnitRef SoftRef)
    {
        return (T*)(SoftRef.lock().get());
    }

    // Returns true if not marked for destruction
    bool IsValid() const { return mSelfRef.lock() != nullptr; }
    HardUnitRef GetSelfRef() const 
    { 
        HardUnitRef RetRef = mSelfRef.lock();
        return RetRef;
    }

private:
    SoftUnitRef mSelfRef;

    virtual void OnCreated(HardUnitRef UnitRef);
    virtual void OnDestroyed() {}

    friend class AvalonMemory;
};

class AvalonMemory
{
public:
    template <class T>
    static void NewUnit(HardUnitRef& NewUnit)
    {
        NewUnit = std::make_shared<T>();
        NewUnit->OnCreated(NewUnit);
    }

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
