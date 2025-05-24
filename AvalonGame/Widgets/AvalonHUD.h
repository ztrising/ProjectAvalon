/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/
/***************************************************************************************
*  Avalon HUD
*  The widget management class owned by the Viewport Manager
****************************************************************************************/

#pragma once

#include "WidgetTypes.h"
#include "../Gameplay/Utility/EventDispatcher.h"
#include "../../AvalonInput/AvalonInputTypes.h"
#include "../AvalonUnit/AvalonUnit.h"

struct FCoord;
struct FFrameBuffer;
class Traveller;

class AvalonHUD : public IAvalonUnit
{
public:
    AvalonHUD();
    ~AvalonHUD();

    static AvalonHUD& GetHUD();
    static void InitializeHUD(FCoord ViewportSize);
    void HandleGameLoaded();

    void OpenContainer(class ContainerComponent* Container) {};
    void CloseContainer(class ContainerComponent* Container) {};

    FFrameBuffer* mBuffer = nullptr;
    HardUnitRef mBaseWidget;

    void TickAnimation(float DeltaSeconds);
    void UpdateRenderState(bool& ForceRedraw);

	/***************************************************************************************
	*  HUD Context
	****************************************************************************************/
public:
    HUDContext GetHUDContext() const { return mHUDContext; }
    void SetHUDContext(HUDContext NewContext);

private:
    HUDContext mHUDContext = EHUDContext::TITLESCREEN;
	/****************************************************************************************/

    /***************************************************************************************
    *  IEventListener
    ****************************************************************************************/
public:
    static void HandleInputKeyEvent(IAvalonUnit* Listener, const FInputKeyEventParams& EventParams);
    static void HandleInputMouseEvent(IAvalonUnit* Listener, const FInputMouseEventParams& EventParams);
    static void OnStartTravel(IAvalonUnit* Listener, Traveller* InTraveller);
    static void OnFinishTravel(IAvalonUnit* Listener, Traveller* InTraveller);
    /****************************************************************************************/

    // Widget Utils
public:
    SoftUnitRef mFocusWidget;

public:
    FFrameBuffer* mDebugBuffer = nullptr;
    bool mDebugBufferChanged = false;
    bool mWantsDebugBufferClear = false;
    FCoord mCachedMousePos; // For debugging
};

