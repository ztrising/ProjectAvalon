/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "WidgetTypes.h"
#include "../Gameplay/Utility/EventDispatcher.h"
#include "../../AvalonInput/AvalonInputTypes.h"
#include "../AvalonUnit/AvalonUnit.h"

struct FCoord;
struct FFrameBuffer;
class Traveller;

class AvalonHUD : public IEventListener
{
public:
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
    static void HandleInputKeyEvent(IEventListener* Listener, const FInputKeyEventParams& EventParams);
    static void HandleInputMouseEvent(IEventListener* Listener, const FInputMouseEventParams& EventParams);
    static void OnStartTravel(IEventListener* Listener, Traveller* InTraveller);
    static void OnFinishTravel(IEventListener* Listener, Traveller* InTraveller);
    /****************************************************************************************/

    // Widget Utils
public:
    SoftUnitRef mFocusWidget;

public:
    FFrameBuffer* mDebugBuffer = nullptr;
    bool mDebugBufferChanged = false;
    bool mWantsDebugBufferClear = false;
    FCoord mCachedMousePos; // For debugging

    //////////////////////////////////////////////////////////////////////////
    //  Singleton Implementation
public:
    static AvalonHUD& GetHUD()
    {
        static AvalonHUD TheHUD;
        return TheHUD;
    }

    AvalonHUD(AvalonHUD const&) = delete;
    void operator=(AvalonHUD const&) = delete;

private:
    AvalonHUD();
    ~AvalonHUD();
    //////////////////////////////////////////////////////////////////////////
};

