/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "WidgetTypes.h"
#include "../../AvalonUtilities/AvalonTypes.h"
#include "AvalonHUD.h"
#include "Animation/WidgetAnimation.h"
#include "../../AvalonViewport/FrameBuffer/FrameBuffer.h"
#include "../AvalonUnit/AvalonUnit.h"
#include "../../AvalonInput/AvalonInputTypes.h"

#include <vector>

// Visual and/or Interactive Elements
//  A Button is Widget that listens for the Confirm input
//  or the mouse coords selecting it.
//  It could have one changing visual element
//  or multiple elements that we swap.

// Widgets Should have Callbacks for their Screens
// or Parent Widgets

// Some widgets are just visuals
//  Like frames, or backgrounds

class IAvalonAnimation;

class AvalonWidget : public IAvalonUnit
{
public:
	AvalonWidget();
	~AvalonWidget();

	// Sets up the visuals of the Widget, called right after creation
	virtual void Construct(const char* WidgetAsset = nullptr);
	virtual void Teardown() {}
	virtual void OnGameLoaded();
	virtual void OnHUDContextChanged(HUDContext NewContext);

	virtual void Show();
	virtual void Hide();

	/***************************************************************************************
	*  Rendering
	****************************************************************************************/
public:
	FCoord GetDimensions();
	FFrameBuffer mBuffer;
	bool IsVisible() const { return mIsVisible; }
	bool CanBeShown() const;

protected:
	bool mStartVisible = true;
	HUDContextMask mValidHUDContexts = EHUDContext::ALL;

private:
	void UpdateRenderState(bool& ForceRedraw);
	void UpdateVisibility();
	bool mNeedsRedraw = false;
	bool mIsVisible = false;
	bool mWantsVisible = true;
	/****************************************************************************************/

	/***************************************************************************************
	*  Positioning
	****************************************************************************************/
public:
	void SetPosition(FCoord NewPos); // TODO : Rename "Set Local Position"

	// TODO : This all should just live in the Frame Buffer, and the "Set Location Position" 
	// should feed into it!
	void RecalculatePosition();

	// Wants to be private
	FPositionData mPositionData = FPositionData();

protected:
	bool IsPositionColliding(FCoord Pos);
	/****************************************************************************************/

	/***************************************************************************************
	*  Text Utilities
	****************************************************************************************/
public:

	// TODO : Remove SetText/SetTextBox, "Set Text With Settings" does it better
	void SetText(const char* Text
				, FCoord Pos
				, unsigned short Attributes = 0
				, bool ApplyAttribute = false
				, bool ClearBuffer = false);

	void SetTextBox(const char* Text,
					FCoord Pos,
					FCoord Extent, bool UseBufferExtent = false, bool FillExtent = true);

	void SetTextWithSettings(const char* Text, const FTextSettings& Settings);
	void DrawDebugText(const char* Text, FCoord Pos);

	void ApplyAttributes(unsigned short Attributes);
	/****************************************************************************************/

	/***************************************************************************************
	*  Focus
	****************************************************************************************/
protected:
	FUnitHandle UpdateFocus(const FCoord& MousePosition);

	virtual void OnFocusGained() {}
	virtual void OnFocusLost() {}

	bool mWantsFocus = false;
	bool mHasFocus = false;
	/****************************************************************************************/

	/***************************************************************************************
	*  Animation
	****************************************************************************************/
public:
	void PlayAnimation(FBufferAnimSettings Settings);
	IAvalonAnimation* MoveTo(FMoveAnimSettings Settings);
	void StopAnimations();

private:
	void TickAnimation(float DeltaSeconds);
	std::vector<IAvalonAnimation*> mRunningAnimations;
	/****************************************************************************************/

	/***************************************************************************************
	*  Input
	****************************************************************************************/
public:
	virtual bool HandleInput(const FInputKeyEventParams& EventParams) { return false; }

private:
	bool HandleInput_Internal(const FInputKeyEventParams& EventParams);
	/****************************************************************************************/

	/***************************************************************************************
	*  Memory Management
	****************************************************************************************/
public:
	template <class T>
	FUnitHandle AddChild(const char* WidgetAsset = nullptr)
	{
		FUnitHandle NewHandle = AvalonMemory::NewUnit<T>();
		AddChild_Internal(NewHandle, WidgetAsset);
		return NewHandle;
	}

	template <class T>
	void RemoveChild(FUnitHandle& Handle)
	{
		RemoveChild_Internal(Handle);
		AvalonMemory::DestroyUnit(Handle.Get<T>());
		//AvalonMemory::DestroyUnit<T>(Handle);
	}

	void SetParent(const FUnitHandle& Handle) { mParent = Handle; }
	FUnitHandle& GetParent() { return mParent; }

private:
	void AddChild_Internal(const FUnitHandle& ChildHandle, const char* WidgetAsset);
	void RemoveChild_Internal(const FUnitHandle& Handle);

	FUnitHandle mParent;
	std::vector<FUnitHandle> mChildren;
	/****************************************************************************************/

	/* Debugging Helpers */
protected:
	void SetAvalonWidgetDebugName(const char* DebugName);

private:
#define AVALON_WIDGET_NAME_SIZE 30
	char WidgetDebugName[AVALON_WIDGET_NAME_SIZE];
	const char* GetAvalonWidgetDebugName() { return WidgetDebugName; }
	/****************************************************************************************/

	friend class AvalonHUD;
};


