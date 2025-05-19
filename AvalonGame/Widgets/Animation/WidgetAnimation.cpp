/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "WidgetAnimation.h"
#include "../../../AvalonViewport/FrameBuffer/FrameBuffer.h"
#include "../../../AvalonViewport/AvalonViewportManager.h"
#include "../AvalonWidget.h"
#include "../WidgetLoader.h"

#define AVALON_DISPLAY_ANIM_ALPHA 0
#if AVALON_DISPLAY_ANIM_ALPHA
#include <stdio.h>
#include <string.h>
#endif //AVALON_DISPLAY_ANIM_ALPHA


void IAvalonAnimation::OnCreated()
{

}

void IAvalonAnimation::OnDestroyed()
{
	StopAnimation();
}

void IAvalonAnimation::PlayAnimation()
{
	mIsPlaying = true;
	mTimePlaying = 0.f;

	OnAnimStarted();
}

void IAvalonAnimation::StopAnimation()
{
	if (mIsPlaying)
	{
		OnAnimStopped();
		mOnAnimEnded.BroadcastEvent(this);
		mIsPlaying = false;
	}
}

bool IAvalonAnimation::Update(float DeltaSeconds)
{
	if (/*this->IsValid() == false || */  mIsPlaying == false)
	{
		return true; // I'm marked for destruction
	}

	bool Finished = false;
	mTimePlaying += DeltaSeconds;
	if (mTimePlaying > 0.f)
	{
		float Alpha = (mTimePlaying / mLength);
		if (Alpha > mEndAlpha)
		{
			Finished = true;
			Alpha = mEndAlpha;
		}

		if (Finished)
		{
			StopAnimation();
		}
		else
		{
			OnAnimUpdated(Alpha);
		}
	}

	return Finished;
}

/***************************************************************************************
*  BufferAnimation_WipeOverTime - Base class for our over time 'Fades'
****************************************************************************************/

BufferAnimation_WipeOverTime::~BufferAnimation_WipeOverTime()
{
	delete mStartState;
	delete mEndState;
}

void BufferAnimation_WipeOverTime::OnAnimStarted()
{
	
}

void BufferAnimation_WipeOverTime::OnAnimStopped()
{
	AssetLoaders::CopyFrameBufferTo(*mEndState, *mTargetBuffer);
}

void BufferAnimation_WipeOverTime::OnAnimUpdated(float Alpha)
{
	unsigned int BufferSize = mTargetBuffer->GetLength();
	for (unsigned int Index = 0; Index < BufferSize; ++Index)
	{
		FFrameBuffer* Source = SourceFromEnd(Alpha, Index) ? mEndState : mStartState;
		(*mTargetBuffer)[Index].mChar = (*Source)[Index].mChar;
		(*mTargetBuffer)[Index].mAttributes = (*Source)[Index].mAttributes;
	}

#if AVALON_DISPLAY_ANIM_ALPHA
	char DebugString[20];
	snprintf(DebugString, 20, "[%.2f]", Alpha);
	AssetLoader::LinearWriteToBuffer(mTargetBuffer, DebugString, FCoord(), AVALON_FG_WHITE, true);
#endif //AVALON_DISPLAY_ANIM_ALPHA
}

bool BufferAnimation_WipeOverTime::SourceFromEnd(float Alpha, int BufferIndex)
{
	bool RetValue = false;
	switch (mSettings.mDirection)
	{
	case EAnimationDirection::DrizzleDown:
	{
		int AlphaIndex = Alpha * mTargetBuffer->GetLength();
		RetValue = BufferIndex <= AlphaIndex;
	}
	break;
	case EAnimationDirection::DrizzleUp:
	{
		int AlphaIndex = (1.f - Alpha) * mTargetBuffer->GetLength();
		RetValue = BufferIndex >= AlphaIndex;
	}
	break;
	case EAnimationDirection::TopToBottom:
	{
		int AlphaIndex = mTargetBuffer->GetSize().X + (Alpha * mTargetBuffer->GetSize().Y);
		RetValue = BufferIndex < AlphaIndex;
	}
	break;
	}

	return RetValue;
}

void BufferAnimation_WipeOverTime::InitAnimation(FFrameBuffer* TargetBuffer, FBufferAnimSettings Settings)
{
	mTargetBuffer	= TargetBuffer;
	mSettings		= Settings;
	mLength			= mSettings.mLength;
	mEndAlpha		= Settings.mEndAlpha;

	switch (mSettings.mStyle)
	{
	case EAnimationStyle::FadeIn:
	{
		mStartState = new FFrameBuffer();
		mStartState->AllocateBuffer(mTargetBuffer->GetSize(), AVALON_CHAR_TRANSPARENT);

		mEndState = new FFrameBuffer();
		AssetLoaders::CopyFrameBufferTo(*mTargetBuffer, *mEndState);
	}
	break;
	case EAnimationStyle::FadeOut:
	{
		mStartState = new FFrameBuffer();
		AssetLoaders::CopyFrameBufferTo(*mTargetBuffer, *mStartState);

		mEndState = new FFrameBuffer();
		mEndState->AllocateBuffer(mTargetBuffer->GetSize(), AVALON_CHAR_TRANSPARENT);
	}
	break;
	case EAnimationStyle::FadeTo:
	{
		mStartState = new FFrameBuffer();
		AssetLoaders::CopyFrameBufferTo(*mTargetBuffer, *mStartState);

		mEndState = new FFrameBuffer();
		AssetLoaders::LoadBufferForAnimation(mSettings.mEndStateAsset, *mEndState);
	}
	break;
	}
}

/***************************************************************************************
*  Animation_MoveTo
****************************************************************************************/
void Animation_MoveTo::InitMoveTo(FMoveAnimSettings Settings)
{
	mLength = Settings.mLength;
	mSettings = Settings;
}

void Animation_MoveTo::OnAnimUpdated(float Alpha)
{
	float PosX = mSettings.mStartLocation.X 
				+ float(mSettings.mEndLocation.X - mSettings.mStartLocation.X) * Alpha;

	float PosY = mSettings.mStartLocation.Y 
				+ float(mSettings.mEndLocation.Y - mSettings.mStartLocation.Y) * Alpha;

	FCoord Location = FCoord( PosX, PosY);

#if AVALON_DISPLAY_ANIM_ALPHA
	char DebugString[20];
	snprintf(DebugString, 20, "[%.2f, %.2f]", PosX, PosY);
	AssetLoader::LinearWriteToBuffer(mSettings.Handle.Get()->mBuffer, DebugString, FCoord(), AVALON_FG_WHITE, true);
#endif //AVALON_DISPLAY_ANIM_ALPHA

	mSettings.mWidgetHandle.Get<AvalonWidget>()->SetPosition(Location);
}

void Animation_MoveTo::OnAnimStarted()
{
	mSettings.mWidgetHandle.Get<AvalonWidget>()->SetPosition(mSettings.mStartLocation);
}

void Animation_MoveTo::OnAnimStopped()
{
	mSettings.mWidgetHandle.Get<AvalonWidget>()->SetPosition(mSettings.mEndLocation);

	//gross
	AvalonViewportManager::GetViewportManager().RequestRedraw();
	// end gross
}
