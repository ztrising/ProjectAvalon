/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../WidgetTypes.h"
#include "../../../AvalonUtilities/AvalonTypes.h"
#include "../../Gameplay/Utility/EventDispatcher.h"
#include "../../AvalonUnit/AvalonUnit.h"

struct FFrameBuffer;

enum class EAnimationStyle
{
	FadeIn,
	FadeOut,
	FadeTo
};

enum class EAnimationDirection
{
	TopToBottom,
	BottomToTop,
	LeftToRight,
	RightToLeft,
	DrizzleDown,
	DrizzleUp
};

struct FBufferAnimSettings
{
	FBufferAnimSettings() {}
	FBufferAnimSettings(const char* EndStateAsset) : mEndStateAsset(EndStateAsset)
	{

	}

	const char* mEndStateAsset = nullptr;

	EAnimationStyle mStyle = EAnimationStyle::FadeIn;
	EAnimationDirection mDirection = EAnimationDirection::DrizzleDown;

	float mLength = 1.f;
	float mEndAlpha = 1.f;
};

/***************************************************************************************
*  IAvalonAnimation
****************************************************************************************/
class IAvalonAnimation : public IAvalonUnit
{
public:
	/***************************************************************************************
	*  IAvalonUnit
	****************************************************************************************/
	virtual void OnCreated() override;
	virtual void OnDestroyed() override;
	/****************************************************************************************/

	void PlayAnimation();
	void StopAnimation();
	bool Update(float DeltaSeconds);

protected:

	virtual void OnAnimStarted() = 0;
	virtual void OnAnimStopped() = 0;
	virtual void OnAnimUpdated(float Alpha) = 0;


	float mTimePlaying = 0.f;
	float mLength = 1.f;
	float mEndAlpha = 1.f;
	bool mIsPlaying = false;

	/***************************************************************************************
	*   Travel Events!
	****************************************************************************************/
public:
	typedef FEventDispatcher<IAvalonAnimation*> AnimEvent;
	AnimEvent mOnAnimEnded;
	/****************************************************************************************/
};
/****************************************************************************************/

class BufferAnimation_WipeOverTime : public IAvalonAnimation
{
	/***************************************************************************************
	*  IBufferAnimation
	****************************************************************************************/
protected:
	void OnAnimStarted() override;
	void OnAnimStopped() override;
	void OnAnimUpdated(float Alpha) override;
	/****************************************************************************************/

public:
	~BufferAnimation_WipeOverTime();

	void InitAnimation(FFrameBuffer* TargetBuffer, FBufferAnimSettings Settings);

private:
	bool SourceFromEnd(float Alpha, int BufferIndex);

	FBufferAnimSettings mSettings;

	FFrameBuffer* mTargetBuffer = nullptr;
	FFrameBuffer* mStartState = nullptr;
	FFrameBuffer* mEndState = nullptr;
};


// Actually, make Animations have multiple properties:
// Character Changes
// Attribute Changes
// Positional Changes
//   Widget Relative (affects children)

struct FMoveAnimSettings
{
	FUnitHandle mWidgetHandle = FUnitHandle();
	FCoord mStartLocation = FCoord();
	FCoord mEndLocation = FCoord();
	float mLength = 1.f;
};

class Animation_MoveTo : public IAvalonAnimation
{
	/***************************************************************************************
	*  IBufferAnimation
	****************************************************************************************/
protected:
	void OnAnimStarted() override;
	void OnAnimStopped() override;
	void OnAnimUpdated(float Alpha) override;
	/****************************************************************************************/

public:
	void InitMoveTo(FMoveAnimSettings Settings);

private:
	FMoveAnimSettings mSettings;
};
