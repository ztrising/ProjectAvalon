/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonInputTypes.h"
#include "../AvalonUtilities/AvalonTypes.h"

#include <vector>

class AvalonInputManager
{
public:

    void Initialize();
    void ProcessInput();

    bool IsKeyDown(EInputEvent Event);

private:

	class AvalonInputDevice* mInputDevice = nullptr;

    FAvalonCurrentInputState mInputState;
    FCoord mCurrentMousePosition;

    /***************************************************************************************
    *   Input Events!
    ****************************************************************************************/
public:
    InputKeyEvent   mOnKeyStateChange;
    InputMouseEvent mOnMouseStateChange;
    /****************************************************************************************/

//////////////////////////////////////////////////////////////////////////
//  Singleton Implementation
public:
    static AvalonInputManager& GetInputManager()
    {
        static AvalonInputManager InputManager;
        return InputManager;
    }

    AvalonInputManager(AvalonInputManager const&) = delete;
    void operator=(AvalonInputManager const&) = delete;

private:
    AvalonInputManager();
//////////////////////////////////////////////////////////////////////////

};
