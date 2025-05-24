/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "FrameBuffer/FrameBuffer.h"
#include "../AvalonUtilities/DynamicArray.h"

#include "../AvalonGame/AvalonUnit/AvalonUnit.h"

class IViewportElement;
class IAvalonAnimation;
struct FFrameBuffer;

class AvalonViewportManager
{
public:
    void Initialize();
    void Teardown();

    void Draw(float DeltaSeconds);

    FCoord GetViewportSize();
    void RequestRedraw() { mForceRedraw = true; }

    void* GetInputHandle();

private:

    class IAvalonViewport* mCurrentViewport = nullptr;
    bool mForceRedraw = false; 

    //////////////////////////////////////////////////////////////////////////
    //  HUD ref
public:
    class AvalonHUD* GetHUDInstance();

private:
    HardUnitRef mHUDRef;
    //////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////
    //  Singleton Implementation
public:
    static AvalonViewportManager& GetViewportManager()
    {
        static AvalonViewportManager ViewportManager;
        return ViewportManager;
    }

    AvalonViewportManager(AvalonViewportManager const&) = delete;
    void operator=(AvalonViewportManager const&) = delete;

private:
    AvalonViewportManager() {};
    ~AvalonViewportManager();
    //////////////////////////////////////////////////////////////////////////
};

