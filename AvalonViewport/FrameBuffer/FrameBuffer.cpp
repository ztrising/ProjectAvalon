/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "FrameBuffer.h"
#include "../../AvalonGame/AssetLoaders/AssetLoader.h"

FFrameBuffer* FFrameBuffer::DrawBuffer = nullptr;
FFrameBuffer* FFrameBuffer::DebugDrawBuffer = nullptr;

FFrameBuffer::~FFrameBuffer()
{
	FreeCells();
}

void FFrameBuffer::FreeCells()
{
	if (mCells != nullptr)
	{
		delete[] mCells;
	}

	mCells = nullptr;
}

void FFrameBuffer::AllocateBuffer(FCoord Size, unsigned short FillChar, unsigned short Attributes)
{
	FreeCells();

	unsigned int BufferSize = Size.X * Size.Y;
	mSize = Size;

	mCells = new FCellData[BufferSize]();
	for (unsigned int Index = 0; Index < BufferSize; ++Index)
	{
		FCellData& CellData = mCells[Index];
		CellData.mChar = FillChar;
		CellData.mAttributes = Attributes;
	}
}

void FFrameBuffer::FillBuffer(unsigned short Char)
{
	unsigned int BufferSize = mSize.X * mSize.Y;
	for (unsigned int Index = 0; Index < BufferSize; ++Index)
	{
		mCells[Index].mChar = Char;
	}
}

bool SetDataValueCallback(FCellData& TargetCell, const FCellData& SourceCell)
{
	bool ChangedValue = false;

	if (TargetCell.mChar != SourceCell.mChar)
	{
		ChangedValue = true;
		TargetCell.mChar = SourceCell.mChar;
	}

	if (TargetCell.mAttributes != SourceCell.mAttributes)
	{
		ChangedValue = true;
		TargetCell.mAttributes = SourceCell.mAttributes;
	}

	return ChangedValue;
}

/*static*/ bool FFrameBuffer::CollapseBuffersDown(const FFrameBuffer& TopBuffer, FFrameBuffer& BottomBuffer, bool ShouldResize)
{
	bool ValueChange = false;

	// This should only happen on load/construction
	if (ShouldResize)
	{
		bool NeedsResize = !TopBuffer.IsWithin(BottomBuffer);
		if (NeedsResize)
		{
			ValueChange = true;

			// New Position is the Top Left most position
			FCoord NewBufferPos = FCoord();
			NewBufferPos.X = (TopBuffer.mPos.X < BottomBuffer.mPos.X) ? TopBuffer.mPos.X : BottomBuffer.mPos.X;
			NewBufferPos.Y = (TopBuffer.mPos.Y < BottomBuffer.mPos.Y) ? TopBuffer.mPos.Y : BottomBuffer.mPos.Y;

			FCoord BottomRight = FCoord();
			FCoord TopBR = TopBuffer.GetBottomRight();
			FCoord BottomBR = BottomBuffer.GetBottomRight();
			BottomRight.X = TopBR.X > BottomBR.X ? TopBR.X : BottomBR.X;
			BottomRight.Y = TopBR.Y > BottomBR.Y ? TopBR.Y : BottomBR.Y;

			//New Extent is BottomLeft - TopLeft
			FCoord NewBufferExtent = FCoord();
			NewBufferExtent.X = BottomRight.X - NewBufferPos.X;
			NewBufferExtent.Y = BottomRight.Y - NewBufferPos.Y;

			FFrameBuffer* NewBuffer = new FFrameBuffer();
			NewBuffer->AllocateBuffer(NewBufferExtent, AVALON_CHAR_TRANSPARENT);
			NewBuffer->SetPos(NewBufferPos);

			// Fill the new larger buffer with the current contents of the bottom buffer
			CollapseBuffersDown(BottomBuffer, *NewBuffer);

			// Then point the bottom buffer at the new buffer
			BottomBuffer.FreeCells();
			BottomBuffer.mCells = NewBuffer->mCells;
			BottomBuffer.mPos   = NewBuffer->mPos;
			BottomBuffer.mSize  = NewBuffer->mSize;

			NewBuffer->mCells = nullptr;
			delete NewBuffer;
			NewBuffer = nullptr;
		}
	}

	short TopWidth = TopBuffer.GetSize().X;
	FCoord TopPos  = TopBuffer.GetPos();

	short BottomWidth = BottomBuffer.GetSize().X;
	unsigned int BottomSize = BottomBuffer.GetLength();

	// For each cell in ToAdd, we find where that cell would be
	// in the outer buffer, and copy the value if it isn't null
	for (int Index = 0; Index < TopBuffer.GetLength(); ++Index)
	{
		const FCellData& SourceData = TopBuffer[Index];
		if (SourceData.mChar != 0 && SourceData.mChar != '$')
		{
			int SourceX = (Index % TopWidth) + TopPos.X;
			int SourceY = (Index / TopWidth) + TopPos.Y;

			int ConvertedIndex = (SourceY * BottomWidth) + SourceX;

			if (ConvertedIndex >= 0 && ConvertedIndex < BottomSize)
			{
				FCellData& TargetCell = BottomBuffer.mCells[ConvertedIndex];
				ValueChange = SetDataValueCallback(TargetCell, SourceData) || ValueChange;
			}
		}
	}

	return ValueChange;
}

bool FFrameBuffer::IsWithin(const FFrameBuffer& Outer) const
{
	// I'm more on the left
	if (mPos.X < Outer.mPos.X)
	{
		return false;
	}

	// I'm above
	if (mPos.Y < Outer.mPos.Y)
	{
		return false;
	}


	FCoord MyBottomRight = GetBottomRight();
	FCoord OuterBottomRight = Outer.GetBottomRight();

	// I'm wider
	if (MyBottomRight.X > OuterBottomRight.X)
	{
		return false;
	}

	//I'm taller
	if (MyBottomRight.Y > OuterBottomRight.Y)
	{
		return false;
	}

	return true;
}

void FFrameBuffer::SetCellData(FCoord mBufferPosition, unsigned short Char, unsigned short Attributes)
{
	FCoord Buffer = GetSize();
	unsigned int BufferSize = Buffer.X * Buffer.Y;

	unsigned int ConvertedIndex = (mBufferPosition.Y * Buffer.X) + mBufferPosition.X;

	if (ConvertedIndex >= 0 && ConvertedIndex < BufferSize)
	{
		FCellData& TargetCell = mCells[ConvertedIndex];
		TargetCell.mChar = Char;
		TargetCell.mAttributes = Attributes;
	}
}

FCoord FFrameBuffer::GetWorldPosition(const FCoord& Extent
									, const FCoord& ParentPos
									, const FCoord& ParentExtent
									, const FPositionData& PosData)
{
	short Pivot_Adjustment_X = PosData.PivotX * Extent.X;
	short Pivot_Adjustment_Y = PosData.PivotY * Extent.Y;

	short Position_X = PosData.Offset.X - Pivot_Adjustment_X;
	short Position_Y = PosData.Offset.Y - Pivot_Adjustment_Y;

	const FCoord AnchorPos		= PosData.ViewportRelative ? FCoord() : ParentPos;
	const FCoord AnchorExtent	= PosData.ViewportRelative ? DrawBuffer->GetSize() : ParentExtent;

	switch (PosData.HRule)
	{
	case EHorizontalAnchor::ELeft:	 Position_X += AnchorPos.X;
		break;
	case EHorizontalAnchor::ECenter: Position_X += AnchorPos.X + (AnchorExtent.X / 2);
		break;
	case EHorizontalAnchor::ERight:  Position_X += AnchorPos.X + AnchorExtent.X;
		break;
	}

	switch (PosData.VRule)
	{
	case EVerticalAnchor::ETop:		Position_Y += AnchorPos.Y;
		break;
	case EVerticalAnchor::ECenter:	Position_Y += AnchorPos.Y + (AnchorExtent.Y / 2);
		break;
	case EVerticalAnchor::EBottom:  Position_Y += AnchorPos.Y + AnchorExtent.Y;
		break;
	}
	
	return FCoord(Position_X, Position_Y);
}
