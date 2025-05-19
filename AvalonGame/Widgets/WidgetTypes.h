/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../../AvalonUtilities/AvalonTypes.h"

namespace EHUDContext
{
	const int TITLESCREEN	= 0x0010;
	const int LOCATION		= 0x0020;
	const int TRAVELLING	= 0x0040;
	const int UNUSED_1		= 0x0080;

	const int ALL = TITLESCREEN | LOCATION | TRAVELLING;
}

typedef unsigned short HUDContext;
typedef unsigned short HUDContextMask;

enum class ETextJustification
{
	Left,
	Center,
	Right
};

struct FTextSettings
{
	FCoord mOffset;
	FCoord mExtent;
	ETextJustification mJustified = ETextJustification::Left;
	bool mUseBufferExtentWidth = false;
	bool mUseBufferExtentHeight = false;
	bool mFillExtent = false;
	unsigned short mFillChar = AVALON_CHAR_BLANK;
	bool mApplyAttributes = false;
	unsigned short mAttributes = AVALON_FG_WHITE | AVALON_BG_BLACK;
};
