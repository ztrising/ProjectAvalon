/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonInputTypes.h"

struct FCoord;

// Interface for classes that want to listen to input
class IInputListener
{
public:
    virtual bool HandleInputKeyEvent(EInputEvent Event, bool NewKeyDownValue) = 0;
    virtual void HandleMouseMoved(FCoord MousePosition) = 0;

    static void StartListeningForInput(IInputListener* Listener);
    static void StopListeningForInput(IInputListener* Listener);

protected:
    void SetWantsInput(bool WantsInput) { mWantsInput = WantsInput; }

private:
    bool mWantsInput = false;
    bool mIsListeningForInput = false;
};

