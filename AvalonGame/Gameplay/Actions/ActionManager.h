/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonAction.h"
#include "../Utility/EventDispatcher.h"

class ActionManager
{
    /***************************************************************************************
    *  Actions!
    ****************************************************************************************/
public:
    void UpdateCurrentActions();
    void SetActionFocus(HardUnitRef NewFocus);
    void ClearActionFocus();

private:
    void PopulateActions();

    FActionState mActionState;
    /****************************************************************************************/

    /***************************************************************************************
    *   Action Events!
    ****************************************************************************************/
public:
    typedef FEventDispatcher<const FActionState&> ActionEvent;

    void BindActionEvent(IAvalonUnit* Listener, ActionEvent::Callback& Callback)
    {
        mOnActionsUpdated.BindEvent(Listener, Callback);
    }

private:
    ActionEvent mOnActionsUpdated;
    /****************************************************************************************/

    //////////////////////////////////////////////////////////////////////////
    //  Singleton Implementation
public:
    static ActionManager& Get()
    {
        static ActionManager Instance;
        return Instance;
    }

    ActionManager(ActionManager const&) = delete;
    void operator=(ActionManager const&) = delete;

private:
    ActionManager() {};
    ~ActionManager() {};
    //////////////////////////////////////////////////////////////////////////
    /****************************************************************************************/
};

