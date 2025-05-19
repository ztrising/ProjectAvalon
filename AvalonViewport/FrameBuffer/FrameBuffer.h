/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../../AvalonUtilities/AvalonTypes.h"

// Maybe make this text agnostic and make a text subclass?


struct FFrameBuffer
{
public:
	~FFrameBuffer();

	inline FCoord GetPos()	 const  { return mPos; }
	inline FCoord GetSize()  const  { return mSize; }
	inline void SetPos(FCoord NewPos) { mPos = NewPos; }

	inline       FCellData& operator[](unsigned int index)       { return mCells[index]; }
	inline const FCellData& operator[](unsigned int index) const { return mCells[index]; }

	
	static FFrameBuffer* DrawBuffer;
	static void InitDrawBuffer(FFrameBuffer* Value) { DrawBuffer = Value; }

	static FFrameBuffer* DebugDrawBuffer;
	static void InitDebugDrawBuffer(FFrameBuffer* Value) { DebugDrawBuffer = Value; }

	void AllocateBuffer(FCoord Size, unsigned short FillChar = AVALON_CHAR_TRANSPARENT, unsigned short Attributes = 0);

	static FCoord GetWorldPosition(const FCoord& Extent
								 , const FCoord& ParentPos
								 , const FCoord& ParentExtent
								 , const FPositionData& PosData);

	static bool CollapseBuffersDown(const FFrameBuffer& TopBuffer, FFrameBuffer& BottomBuffer, bool ShouldResize = false);


	void FillBuffer(unsigned short Char = AVALON_CHAR_BLANK);
	bool IsWithin(const FFrameBuffer& Outer) const;

	void SetCellData(FCoord mBufferPosition, unsigned short Char, unsigned short Attributes);


	inline unsigned int GetLength() const { return mSize.X * mSize.Y; }
	inline FCoord GetBottomRight() const { return FCoord(mSize.X + mPos.X, mSize.Y + mPos.Y);  }

	
	inline unsigned int GetIndex(FCoord Coord) { return (Coord.Y * mSize.X) + Coord.X; }




	void FreeCells();

private:
	// Represents a 2D array
	FCellData* mCells = nullptr;

	FCoord mPos = FCoord();
	FCoord mSize = FCoord();

	friend class IViewportElement;
	friend class IAvalonAnimation;
	friend class AssetLoader;
	friend class BufferAnimation_FadeTo;
	friend class AvalonViewportManager;
};

