/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonWidget.h"

#include "../AvalonGame.h" // For the exit test
#include "AvalonHUD.h"
#include "../../AvalonViewport/FrameBuffer/FrameBuffer.h"
#include "../../AvalonViewport/AvalonViewportManager.h"
#include "WidgetLoader.h"

#define AVALON_DISPLAY_COORDS 0
#define AVALON_DISPLAY_WIDGET_FOCUS 1

#if AVALON_DISPLAY_COORDS || AVALON_DISPLAY_WIDGET_FOCUS
#include <stdio.h>
#include <string.h>
#endif //AVALON_DISPLAY_COORDS

AvalonWidget::AvalonWidget()
{
	SetAvalonWidgetDebugName("AvalonWidget");
}

AvalonWidget::~AvalonWidget()
{
	StopAnimations();
}

void AvalonWidget::Construct(const char* WidgetAsset)
{
	if (WidgetAsset != nullptr)
	{
		AssetLoaders::LoadVisualElement(WidgetAsset, this);
		AssetLoaders::LoadWidgetComposition(WidgetAsset, this);
	}

	RecalculatePosition();

	if (mStartVisible)
	{
		Show();
	}
	else
	{
		Hide();
	}
}

void AvalonWidget::OnGameLoaded()
{
	for (auto& ChildHandle : mChildren)
	{
		ChildHandle.Get<AvalonWidget>()->OnGameLoaded();
	}
}

void AvalonWidget::OnHUDContextChanged(HUDContext NewContext)
{
	for (auto& ChildHandle : mChildren)
	{
		ChildHandle.Get<AvalonWidget>()->OnHUDContextChanged(NewContext);
	}

	UpdateVisibility();
}

void AvalonWidget::Show()
{
	mWantsVisible = true;
	UpdateVisibility();
}

void AvalonWidget::Hide()
{
	mWantsVisible = false;
	UpdateVisibility();
}

/***************************************************************************************
*  Rendering
****************************************************************************************/
FCoord AvalonWidget::GetDimensions()
{
	return mBuffer.GetSize();
}

bool AvalonWidget::CanBeShown() const
{
	return mValidHUDContexts & AvalonHUD::GetHUD().GetHUDContext();
}

void AvalonWidget::UpdateRenderState(bool& ForceRedraw)
{
	if (mIsVisible)
	{
		// Bottom first!
		if (FFrameBuffer::DrawBuffer && (&mBuffer) != (FFrameBuffer::DrawBuffer))
		{
			if (ForceRedraw || mNeedsRedraw)
			{
				bool BufferChanged = false;
				{


#if AVALON_DISPLAY_COORDS
					FTextSettings TextSettings;
					TextSettings.mOffset = FCoord();
					TextSettings.mExtent = FCoord(20, 1);
					TextSettings.mJustified = ETextJustification::Left;
					char DebugString[20];
					snprintf(DebugString, 20, "[%d, %d]", mBuffer.GetPos().X, mBuffer.GetPos().Y);
					SetTextWithSettings(DebugString, TextSettings);
#endif //AVALON_DISPLAY_COORDS

					BufferChanged = FFrameBuffer::CollapseBuffersDown(mBuffer, *FFrameBuffer::DrawBuffer);
				}

				ForceRedraw = BufferChanged || ForceRedraw;
			}
		}

		// then the things on TOP
		for (auto& ChildHandle : mChildren)
		{
			ChildHandle.Get<AvalonWidget>()->UpdateRenderState(ForceRedraw);
		}

#if AVALON_DISPLAY_WIDGET_FOCUS
		if (mWantsFocus)
		{
			DrawDebugText(GetAvalonWidgetDebugName(), FCoord(0, 1));
		}
#endif //AVALON_DISPLAY_WIDGET_FOCUS
	}

	mNeedsRedraw = false;
}

void AvalonWidget::UpdateVisibility()
{
	if (mIsVisible)
	{
		if (!CanBeShown() || !mWantsVisible)
		{
			mIsVisible = false;
			AvalonViewportManager::GetViewportManager().RequestRedraw();

			StopAnimations();
		}
	}
	else
	{
		if (CanBeShown() && mWantsVisible)
		{
			mIsVisible = true;
			mNeedsRedraw = true;
		}
	}
}
/****************************************************************************************/

/***************************************************************************************
*  Positioning
****************************************************************************************/
void AvalonWidget::SetPosition(FCoord NewPos)
{
	mPositionData.Offset = NewPos;
	RecalculatePosition();
}

void AvalonWidget::RecalculatePosition()
{
	const FCoord OldPos = mBuffer.GetPos();
	if (const AvalonWidget* Parent = mParent.Get<AvalonWidget>())
	{
		FCoord NewPos = FFrameBuffer::GetWorldPosition(  mBuffer.GetSize()
													   , Parent->mBuffer.GetPos()
													   , Parent->mBuffer.GetSize()
													   , mPositionData);

		mBuffer.SetPos(NewPos);

		mNeedsRedraw = (mBuffer.GetPos() != OldPos);
	}

	// Update children positions
	for (auto& ChildHandle : mChildren)
	{
		ChildHandle.Get<AvalonWidget>()->RecalculatePosition();
	}
}

bool AvalonWidget::IsPositionColliding(FCoord Pos)
{
	FCoord Extent	= mBuffer.GetSize();
	FCoord WorldPos = mBuffer.GetPos();

	if (Pos.X >= WorldPos.X && Pos.X <= (WorldPos.X + Extent.X) &&
		Pos.Y >= WorldPos.Y && Pos.Y <= (WorldPos.Y + Extent.Y))
	{
		return true;
	}

	return false;
}
/****************************************************************************************/

/***************************************************************************************
*  Text Utilities
****************************************************************************************/
void AvalonWidget::SetText(const char* Text
	, FCoord Pos
	, unsigned short Attributes
	, bool ApplyAttribute
	, bool ClearBuffer)
{
	mNeedsRedraw = true;

	if (ClearBuffer)
	{
		mBuffer.FillBuffer(AVALON_CHAR_BLANK);
	}

	AssetLoaders::LinearWriteToBuffer(mBuffer, Text, Pos, Attributes, ApplyAttribute);
}

void AvalonWidget::SetTextBox(const char* Text,
								FCoord Pos,
								FCoord Extent, bool UseBufferExtent, bool FillExtent)
{
	mNeedsRedraw = true;

	FTextSettings Settings;
	Settings.mOffset = Pos;
	Settings.mExtent = Extent;
	Settings.mUseBufferExtentWidth = UseBufferExtent;
	Settings.mUseBufferExtentHeight = UseBufferExtent;
	Settings.mFillExtent = FillExtent;

	AssetLoaders::WriteTextToBuffer(mBuffer, Text, Settings);
}

void AvalonWidget::SetTextWithSettings(const char* Text, const FTextSettings& Settings)
{
	mNeedsRedraw = true;
	AssetLoaders::WriteTextToBuffer(mBuffer, Text, Settings);
}

void AvalonWidget::DrawDebugText(const char* Text, FCoord Pos)
{
	AvalonHUD::GetHUD().mDebugBufferChanged = true;
	AssetLoaders::LinearWriteToBuffer(*FFrameBuffer::DebugDrawBuffer, Text, Pos, AVALON_FG_BLUE, true);
}

void AvalonWidget::ApplyAttributes(unsigned short Attributes)
{
	mNeedsRedraw = true;
	AssetLoaders::ApplyAttributesToBuffer(mBuffer, Attributes);
}
/****************************************************************************************/

/***************************************************************************************
*  Animation
****************************************************************************************/
void AvalonWidget::PlayAnimation(FBufferAnimSettings Settings)
{
	BufferAnimation_WipeOverTime* NewAnim = new BufferAnimation_WipeOverTime();
	NewAnim->InitAnimation(&mBuffer, Settings);

	NewAnim->PlayAnimation();
	mRunningAnimations.push_back(NewAnim);
}

IAvalonAnimation* AvalonWidget::MoveTo(FMoveAnimSettings Settings)
{
	Animation_MoveTo* NewAnim = new Animation_MoveTo();
	Settings.mWidgetHandle = GetSelfHandle();
	NewAnim->InitMoveTo(Settings);
	 
	NewAnim->PlayAnimation();
	mRunningAnimations.push_back(NewAnim);

	return NewAnim;
}

void AvalonWidget::StopAnimations()
{
	for (auto Animation : mRunningAnimations)
	{
		AvalonMemory::DestroyUnit(Animation);
	}

	mRunningAnimations.clear();
}

void AvalonWidget::TickAnimation(float DeltaSeconds)
{
	if (mIsVisible)
	{
		if (mRunningAnimations.size() > 0)
		{
			mNeedsRedraw = true;

			auto It = mRunningAnimations.begin();
			while (It != mRunningAnimations.end())
			{
				IAvalonAnimation* Animation = (*It);
				if (Animation->Update(DeltaSeconds))
				{
					// TODO: Anim events makes updating anims volatile...

					// Updating an animation can cause animations
					// to be stopped and deleted...
					if (mRunningAnimations.size() > 0)
					{
						It = mRunningAnimations.erase(It);
						delete Animation;
					}
					else
					{
						It = mRunningAnimations.end();
					}
				}
				else
				{
					It++;
				}
			}
		}

		for (auto& ChildHandle : mChildren)
		{
			ChildHandle.Get<AvalonWidget>()->TickAnimation(DeltaSeconds);
		}
	}
}
/****************************************************************************************/

/***************************************************************************************
*  Input
****************************************************************************************/
bool AvalonWidget::HandleInput_Internal(const FInputKeyEventParams& EventParams)
{
	for (auto& ChildHandle : mChildren)
	{
		AvalonWidget* Widget = ChildHandle.Get<AvalonWidget>();
		if (Widget->IsVisible() && Widget->HandleInput_Internal(EventParams))
		{
			return true;
		}
	}

	return HandleInput(EventParams);
}
/****************************************************************************************/

FUnitHandle AvalonWidget::UpdateFocus(const FCoord& MousePosition)
{
	if (mIsVisible)
	{
		// The most recently added children are checked first
		// as they are on top
		auto It = mChildren.end();
		while (It != mChildren.begin())
		{
			It--;

			AvalonWidget* Widget = (*It).Get<AvalonWidget>();
			FUnitHandle ChildHandle = Widget->UpdateFocus(MousePosition);
			if (ChildHandle.IsValid())
			{
				return ChildHandle;
			}
		}

		
		if (mWantsFocus)
		{
			bool IsColliding = IsPositionColliding(MousePosition);
			if (IsColliding)
			{
				return GetSelfHandle();
			}
		}
	}

	return FUnitHandle();
}

void AvalonWidget::AddChild_Internal(const FUnitHandle& ChildHandle, const char* WidgetAsset)
{
	mChildren.push_back(ChildHandle);

	AvalonWidget* Widget = ChildHandle.Get<AvalonWidget>();
	Widget->SetParent(GetSelfHandle());
	
	Widget->Construct(WidgetAsset);
	RecalculatePosition();
}

void AvalonWidget::RemoveChild_Internal(const FUnitHandle& Handle)
{
	mNeedsRedraw = true;

	auto It = std::find(mChildren.begin(), mChildren.end(), Handle);
	if (It != mChildren.end())
	{
		mChildren.erase(It);
	}

	RecalculatePosition();

	AvalonViewportManager::GetViewportManager().RequestRedraw();
}

void AvalonWidget::SetAvalonWidgetDebugName(const char* DebugName)
{
	snprintf(WidgetDebugName, AVALON_WIDGET_NAME_SIZE, DebugName);
}