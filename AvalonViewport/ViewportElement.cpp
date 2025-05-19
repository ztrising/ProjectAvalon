/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

/*#include "ViewportElement.h"
#include "FrameBuffer/FrameBuffer.h"
#include "AvalonViewportManager.h"
#include "Animation/BufferAnimation.h"

#include "../AvalonGame/AssetLoaders/AssetLoader.h"

#define AVALON_DISPLAY_COORDS 1
#if AVALON_DISPLAY_COORDS
#include <stdio.h>
#include <string.h>
#endif //AVALON_DISPLAY_COORDS

IViewportElement::IViewportElement()
{
	// This is where we should initialize the buffer...
}

IViewportElement::~IViewportElement()
{
	delete mBuffer;
	mBuffer = nullptr;

	unsigned int NumAnims = mRunningAnimations.Num();
	for (unsigned int Index = 0; Index < NumAnims; ++Index)
	{
		IAvalonAnimation* Animation = mRunningAnimations[Index];
		delete Animation;
		Animation = nullptr;
	}
}

void IViewportElement::LoadAsset(const char* FileName)
{
	//AssetLoader::LoadVisualElement(FileName, this);
	RecalculatePosition();
}

void IViewportElement::UnloadAsset()
{
	delete mBuffer;
	mBuffer = nullptr;

	unsigned int NumAnims = mRunningAnimations.Num();
	for (unsigned int Index = 0; Index < NumAnims; ++Index)
	{
		IAvalonAnimation* Animation = mRunningAnimations[Index];
		delete Animation;
		Animation = nullptr;
	}
	mRunningAnimations.Clear();
}

void IViewportElement::AddChildElement(IViewportElement* Element) 
{ 
	mChildViewportElements.Add(Element); 
	Element->mParent = this;
	Element->RecalculatePosition();
}

void IViewportElement::RemoveChildElement(IViewportElement* Element)
{ 
	mChildViewportElements.Remove(Element);
	mNeedsRedraw = true;
}

void IViewportElement::SetPosition(FCoord NewPos)
{
	mPositionData.Offset = NewPos;
	RecalculatePosition();
}

void IViewportElement::SetAttachmentRules(EHorizontalAnchor HorizontalRule, EVerticalAnchor VerticalRule)
{
	mPositionData.HRule = HorizontalRule;
	mPositionData.VRule = VerticalRule;
	RecalculatePosition();
}

void IViewportElement::RecalculatePosition()
{
	if (mBuffer)
	{
		FCoord OldPos = mBuffer->mPos;

		if (mParent && mParent->mBuffer)
		{
			mBuffer->mPos = FFrameBuffer::GetWorldPosition( mBuffer->mSize
													      , mParent->mBuffer->mPos
														  , mParent->mBuffer->mSize
														  , mPositionData);
		}

		mNeedsRedraw = (mBuffer->mPos != OldPos);

		// Update children positions
		unsigned int NumChildren = mChildViewportElements.Num();
		for (unsigned int Index = 0; Index < NumChildren; ++Index)
		{
			mChildViewportElements[Index]->RecalculatePosition();
		}
	}
}

void IViewportElement::TickAnimation(float DeltaSeconds)
{
	if (mIsVisible)
	{
		if (mRunningAnimations.Num() > 0)
		{
			mNeedsRedraw = true;

			int StartIndex = mRunningAnimations.Num() - 1;
			for (int Index = StartIndex; Index >= 0; --Index)
			{
				IAvalonAnimation* Animation = mRunningAnimations[Index];
				if (Animation->Update(DeltaSeconds))
				{
					mRunningAnimations.Remove(Animation);
					delete Animation;
				}
			}
		}

		unsigned int NumChildren = mChildViewportElements.Num();
		for (unsigned int Index = 0; Index < NumChildren; ++Index)
		{
			mChildViewportElements[Index]->TickAnimation(DeltaSeconds);
		}
	}
}

void IViewportElement::UpdateRenderState(bool& ForceRedraw)
{
	if (mIsVisible)
	{
		// Bottom first!
		if (FFrameBuffer::DrawBuffer && mBuffer != FFrameBuffer::DrawBuffer)
		{
			if (ForceRedraw || mNeedsRedraw)
			{
				bool BufferChanged = false;
				if (mBuffer)
				{
#if AVALON_DISPLAY_COORDS
					char DebugString[20];
					snprintf(DebugString, 20, "[%d, %d]", mBuffer->mPos.X, mBuffer->mPos.Y);
					SetText(DebugString, FCoord());
#endif //AVALON_DISPLAY_COORDS

					BufferChanged = FFrameBuffer::CollapseBuffersDown(*mBuffer, *FFrameBuffer::DrawBuffer);
				}

				ForceRedraw = BufferChanged || ForceRedraw;
			}
		}

		// then the things on TOP
		unsigned int NumChildren = mChildViewportElements.Num();
		for (unsigned int Index = 0; Index < NumChildren; ++Index)
		{
			mChildViewportElements[Index]->UpdateRenderState(ForceRedraw);
		}
	}

	mNeedsRedraw = false;
}

void IViewportElement::SetVisible(bool NewVisible)
{
	if (NewVisible != mIsVisible)
	{
		mIsVisible = NewVisible;
		mNeedsRedraw = true;

		if (!mIsVisible)
		{
			AvalonViewportManager::GetViewportManager().RequestRedraw();
		}
	}
}

void IViewportElement::PlayFadeToAnimation(const char* Asset, float Length)
{
	//BufferAnimation_FadeTo* NewAnim = new BufferAnimation_FadeTo();
	//NewAnim->InitFadeTo(Asset, Length, mBuffer);

	//NewAnim->Start();
	//mRunningAnimations.Add(NewAnim);
}

// Is Position Within Frame
bool IViewportElement::IsPositionColliding(FCoord Pos)
{
	if (mBuffer)
	{
		FCoord Extent = mBuffer->GetSize();
		FCoord WorldPos = mBuffer->GetPos();

		if (Pos.X >= WorldPos.X && Pos.X <= (WorldPos.X + Extent.X) &&
			Pos.Y >= WorldPos.Y && Pos.Y <= (WorldPos.Y + Extent.Y))
		{
			return true;
		}
	}

	return false;
}

void IViewportElement::SetText(const char* Text, FCoord Pos, unsigned short Attributes, bool ApplyAttributes, bool ClearBuffer)
{
	if (mBuffer)
	{
		mNeedsRedraw = true;

		if (ClearBuffer)
		{
			mBuffer->FillBuffer(AVALON_CHAR_BLANK);
		}

		//AssetLoaders::LinearWriteToBuffer(mBuffer, Text, Pos, Attributes, ApplyAttributes);
	}
}

void IViewportElement::ApplyAttributes(unsigned short Attributes)
{
	if (mBuffer)
	{
		mNeedsRedraw = true;
		AssetLoader::ApplyAttributesToBuffer(mBuffer, Attributes);
	}
}*/



