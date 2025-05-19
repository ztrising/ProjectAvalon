/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once
/*
#include "../AvalonUtilities/AvalonTypes.h"
#include "../AvalonUtilities/DynamicArray.h"

class IAvalonAnimation;
struct FFrameBuffer;

// Interface for classes that want to render something
// Loads visual data, and maintains a buffer to store it
// Registers itself with the ViewportManager
class IViewportElement
{
public:

	IViewportElement();
	~IViewportElement();

public:
	void SetVisible(bool NewVisible);
	inline bool IsVisible() { return mIsVisible; }

private:
	bool mIsVisible = false;

public:
	void SetPosition(FCoord NewPos);
	void RecalculatePosition();
	void SetAttachmentRules( EHorizontalAnchor HorizontalRule
						   , EVerticalAnchor VerticalRule);

	// Wants to be private
	FPositionData mPositionData = FPositionData();

protected:
	bool IsPositionColliding(FCoord Pos);

public:
	void SetText( const char* Text
				, FCoord Pos
				, unsigned short Attributes = 0
				, bool ApplyAttribute = false
				, bool ClearBuffer = false);

	void ApplyAttributes(unsigned short Attributes);

public:
	void PlayFadeToAnimation(const char* Asset, float Length);

private:
	void TickAnimation(float DeltaSeconds);
	DynamicArray<IAvalonAnimation*> mRunningAnimations;


public:
	// Wants to be private:
	FFrameBuffer* mBuffer = nullptr;
	IViewportElement* mParent = nullptr;

	// Wants to be protected:
	void AddChildElement(IViewportElement* Element);
	void RemoveChildElement(IViewportElement* Element);

protected:

	void LoadAsset(const char* FileName);
	void UnloadAsset();

private:

	void UpdateRenderState(bool& ForceRedraw);
	bool mNeedsRedraw = false;

	DynamicArray<IViewportElement*> mChildViewportElements;

	friend class AvalonViewportManager;
};
*/