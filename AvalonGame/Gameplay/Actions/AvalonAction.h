/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../Actor/ActorTypes.h"
#include "../Utility/Saveable.h"
#include "../../AvalonUnit/AvalonUnit.h"

#include <string>
#include <vector>

enum EActionContext
{
    Travel = 0,
    Dialogue,
    Shop,
    Inventory,
    Container,
    Combat,
    Count
};

struct FEffectContext
{
    SoftUnitRef mSource;
    SoftUnitRef mTarget;
};

class IAvalonEffect : public ISaveable, public IAvalonUnit
{
public:
    virtual void ExecuteEffect(FEffectContext& Context) = 0;

    /***************************************************************************************
    *  ISaveable -  Not all Effects are serializable, so we don't need to implement
    *               these for every one!
    ****************************************************************************************/
    virtual void Load(FSaveContext& Context) override {};
    virtual void Save(FSaveContext& Context) override {};
    /****************************************************************************************/
};

class AvalonAction : public ISaveable, public IAvalonUnit
{
public:
    ~AvalonAction();

    void Execute();

    template <class T>
    T* AddEffect()
    {
        HardUnitRef NewEffectRef;
        AvalonMemory::NewUnit<T>(NewEffectRef);

        mEffects.push_back(NewEffectRef);

        T* Effect = Get<T>(NewEffectRef);
        return Effect;
    }

    static AvalonAction* NewAction(HardUnitRef& OutActionRef)
    {
        AvalonMemory::NewUnit<AvalonAction>(OutActionRef);
        AvalonAction* NewAction = Get<AvalonAction>(OutActionRef);
        return NewAction;
    }

    std::string mActionPrompt;

    HardRefList mEffects;
    //std::vector<IAvalonEffect*> mEffects;

    FEffectContext mContext;

    /***************************************************************************************
    *  ISaveable
    ****************************************************************************************/
private:
    virtual void Load(FSaveContext& Context) override;
    virtual void Save(FSaveContext& Context) override;
    /****************************************************************************************/
};

//typedef std::vector<AvalonAction*> ActionList;
typedef std::vector<HardUnitRef> ActionList;

struct FActionState
{
    EActionContext mContext;
    SoftUnitRef mFocus;
    ActionList mActions;
};

class IActionProvider
{
public:
    virtual void GatherActionsFor(const AvalonActor* Target, ActionList& OutActions) {};
};



