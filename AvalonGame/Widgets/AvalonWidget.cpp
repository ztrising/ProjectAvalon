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
		AvalonWidget* ChildWidget = Get<AvalonWidget>(ChildHandle);
		ChildWidget->OnGameLoaded();
	}
}

void AvalonWidget::OnHUDContextChanged(HUDContext NewContext)
{
	for (auto& ChildHandle : mChildren)
	{
		AvalonWidget* ChildWidget = Get<AvalonWidget>(ChildHandle);
		ChildWidget->OnHUDContextChanged(NewContext);
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
			AvalonWidget* ChildWidget = Get<AvalonWidget>(ChildHandle);
			ChildWidget->UpdateRenderState(ForceRedraw);
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
	AvalonWidget* Parent = IAvalonUnit::Get<AvalonWidget>(mParent);
	if (Parent != nullptr)
	{
		FCoord NewPos = FFrameBuffer::GetWorldPosition(  mBuffer.GetSize()
													   , Parent->mBuffer.GetPos()
													   , Parent->mBuffer.GetSize()
													   , mPositionData);

		mBuffer.SetPos(NewPos);

		mNeedsRedraw = (mBuffer.GetPos() != OldPos);
	}

	// Update children positions
	for (auto& ChildRef : mChildren)
	{
		AvalonWidget* ChildWidget = IAvalonUnit::Get<AvalonWidget>(ChildRef);
		ChildWidget->RecalculatePosition();
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
void AvalonWidget::PlayAnimation(FBufferAnimSettings* Settings)
{
	HardUnitRef AnimationRef;
	AvalonMemory::NewUnit<BufferAnimation_WipeOverTime>(AnimationRef);
	BufferAnimation_WipeOverTime* NewAnim = Get<BufferAnimation_WipeOverTime>(AnimationRef);

	NewAnim->InitAnimation(&mBuffer, Settings);
	NewAnim->PlayAnimation();

	mRunningAnimations.push_back(AnimationRef);
}

IAvalonAnimation* AvalonWidget::MoveTo(FMoveAnimSettings* Settings)
{
	HardUnitRef AnimationRef;
	AvalonMemory::NewUnit<Animation_MoveTo>(AnimationRef);
	Animation_MoveTo* NewAnim = Get<Animation_MoveTo>(AnimationRef);

	Settings->mWidgetRef = GetSelfRef();

	NewAnim->InitMoveTo(Settings);
	NewAnim->PlayAnimation();

	mRunningAnimations.push_back(AnimationRef);

	return NewAnim;
}

void AvalonWidget::StopAnimations()
{
	for (auto& Animation : mRunningAnimations)
	{
		Animation.reset();
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
				IAvalonAnimation* Animation = Get<IAvalonAnimation>(*It);
				if (Animation->Update(DeltaSeconds))
				{
					// TODO: Anim events makes updating anims volatile...

					// Updating an animation can cause animations
					// to be stopped and deleted...
					if (mRunningAnimations.size() > 0)
					{
						It->reset();
						It = mRunningAnimations.erase(It);
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
			AvalonWidget* ChildWidget = Get<AvalonWidget>(ChildHandle);
			ChildWidget->TickAnimation(DeltaSeconds);
		}
	}
}
/****************************************************************************************/

/***************************************************************************************
*  Input
****************************************************************************************/
bool AvalonWidget::HandleInput_Internal(const FInputKeyEventParams& EventParams)
{
	for (auto& ChildRef : mChildren)
	{
		AvalonWidget* ChildWidget = Get<AvalonWidget>(ChildRef);
		if (ChildWidget->IsVisible() && ChildWidget->HandleInput_Internal(EventParams))
		{
			return true;
		}
	}

	return HandleInput(EventParams);
}
/****************************************************************************************/

AvalonWidget* AvalonWidget::UpdateFocus(const FCoord& MousePosition)
{
	AvalonWidget* RetValue = nullptr;

	if (mIsVisible)
	{
		// The most recently added children are checked first
		// as they are on top
		auto It = mChildren.end();
		while (It != mChildren.begin())
		{
			It--;

			AvalonWidget* ChildWidget = IAvalonUnit::Get<AvalonWidget>(*It);
			RetValue = ChildWidget->UpdateFocus(MousePosition);
			if (RetValue != nullptr)
			{
				break;
			}
		}

		
		if (RetValue == nullptr && mWantsFocus)
		{
			bool IsColliding = IsPositionColliding(MousePosition);
			if (IsColliding)
			{
				RetValue = this;
			}
		}
	}

	return RetValue;
}

void AvalonWidget::AddChild_Internal(const HardUnitRef& ChildWidget, const char* WidgetAsset)
{
	mChildren.push_back(ChildWidget);

	AvalonWidget* Child = IAvalonUnit::Get<AvalonWidget>(ChildWidget);

	HardUnitRef SelfRef = GetSelfRef();
	Child->SetParent(SelfRef);
	Child->Construct(WidgetAsset);

	RecalculatePosition();
}

void AvalonWidget::RemoveChild_Internal(const HardUnitRef& ChildWidget)
{
	mNeedsRedraw = true;

	auto It = std::find(mChildren.begin(), mChildren.end(), ChildWidget);
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