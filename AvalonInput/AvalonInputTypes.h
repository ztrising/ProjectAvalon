/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../AvalonGame/Gameplay/Utility/EventDispatcher.h"
#include "../AvalonUtilities/AvalonTypes.h"

#define AVALON_NUM_INPUT_KEYS 10

enum class EInputEvent
{
    ENavUp = 0,
    ENavDown = 1,
    ENavLeft = 2,
    ENavRight = 3,
    EConfirm = 4,
    EBack = 5,
    ELoadGame = 6,
    ENewGame = 7,
    EQuit = 8,
    ELeftMouseButton = 9,
    ERightMouseButton = 10,
};

struct FAvalonInputEventState
{
private:
    bool mIsKeyDown = false;
    bool mChangedThisFrame = false;

public:
    inline void SetKeyDown(bool KeyDown)
    {
        if (!mChangedThisFrame)
        {
            mIsKeyDown = KeyDown;
            mChangedThisFrame = true;
        }
    }

    inline void Processed()
    {
        mChangedThisFrame = false;
    }

    inline bool IsKeyDown() { return mIsKeyDown; }
    inline bool HasChangedThisFrame() { return mChangedThisFrame; }
};

typedef FAvalonInputEventState FAvalonCurrentInputState[AVALON_NUM_INPUT_KEYS];

struct FInputKeyEventParams
{
    EInputEvent mEvent = EInputEvent::EBack;
    bool mKeyDown = false;
};

struct FInputMouseEventParams
{
    FCoord mMousePosition;
};

typedef FEventDispatcher<const FInputKeyEventParams&> InputKeyEvent;
typedef FEventDispatcher<const FInputMouseEventParams&> InputMouseEvent;


