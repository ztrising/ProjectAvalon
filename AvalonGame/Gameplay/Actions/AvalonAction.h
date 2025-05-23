/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../Actor/ActorTypes.h"
#include "../Utility/Saveable.h"

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

class IAvalonEffect : public ISaveable
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

class AvalonAction : public ISaveable
{
public:
    ~AvalonAction();

    void Execute();

    template <class T>
    T* AddEffect()
    {
        T* NewInstance = new T;
        IAvalonEffect* Effect = static_cast<IAvalonEffect*>(NewInstance);
        mEffects.push_back(Effect);

        return NewInstance;
    }

    std::string mActionPrompt;
    std::vector<IAvalonEffect*> mEffects;

    FEffectContext mContext;

    /***************************************************************************************
    *  ISaveable
    ****************************************************************************************/
private:
    virtual void Load(FSaveContext& Context) override;
    virtual void Save(FSaveContext& Context) override;
    /****************************************************************************************/
};

typedef std::vector<AvalonAction*> ActionList;

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



