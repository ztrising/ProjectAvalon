/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#define AVALON_FG_BLACK		0x0000 // let's get pedantic
#define AVALON_FG_BLUE      0x0001 // text color contains blue.
#define AVALON_FG_GREEN     0x0002 // text color contains green.
#define AVALON_FG_RED       0x0004 // text color contains red.
#define AVALON_FG_INTENSITY 0x0008 // text color is intensified.

#define AVALON_FG_WHITE		(AVALON_FG_BLUE | AVALON_FG_GREEN | AVALON_FG_RED)
#define AVALON_FG_CYAN		(AVALON_FG_BLUE | AVALON_FG_GREEN)
#define AVALON_FG_MAGENTA	(AVALON_FG_BLUE | AVALON_FG_RED)
#define AVALON_FG_YELLOW	(AVALON_FG_RED | AVALON_FG_GREEN)

#define AVALON_BG_BLACK		0x0000 // let's get pedantic
#define AVALON_BG_BLUE      0x0010 // background color contains blue.
#define AVALON_BG_GREEN     0x0020 // background color contains green.
#define AVALON_BG_RED       0x0040 // background color contains red.
#define AVALON_BG_INTENSITY 0x0080 // background color is intensified.

#define AVALON_BG_WHITE		(AVALON_BG_BLUE | AVALON_BG_GREEN | AVALON_BG_RED)
#define AVALON_BG_CYAN		(AVALON_BG_BLUE | AVALON_BG_GREEN)
#define AVALON_BG_MAGENTA	(AVALON_BG_BLUE | AVALON_BG_RED)
#define AVALON_BG_YELLOW	(AVALON_BG_RED | AVALON_BG_GREEN)

#define AVALON_CHAR_TRANSPARENT '$'
#define AVALON_CHAR_BLANK ' '


#define AVALON_CELL_DEFAULT 0 //(AVALON_BG_BLUE | AVALON_BG_GREEN | AVALON_FG_BLUE | AVALON_FG_GREEN)

#define AVALON_MAX_TEXT_SML 32
#define AVALON_MAX_TEXT_MED_SML 64
#define AVALON_MAX_TEXT_MED 128
#define AVALON_MAX_TEXT_LRG 256
#define AVALON_MAX_TEXT_XLRG 512

struct FCoord
{
	FCoord() : X(0), Y(0) {}
	FCoord(short NewX,short NewY) : X(NewX), Y(NewY) {}

	short X;
	short Y;

	inline FCoord operator+(const FCoord& rhs) const { return FCoord(X + rhs.X, Y + rhs.Y); }
	inline FCoord operator-(const FCoord& rhs) const { return FCoord(X - rhs.X, Y - rhs.Y); }
	inline bool operator==(const FCoord& rhs) { return (X == rhs.X) && (Y == rhs.Y); }
	inline bool operator!=(const FCoord& rhs) { return (X != rhs.X) || (Y != rhs.Y); }
	inline FCoord& operator+=(const FCoord& rhs) { X += rhs.X; Y += rhs.Y; return *this; }
};

struct FCellData
{
	unsigned short mChar;           // Unicode friendly
	unsigned short mAttributes;     // Foreground/background color
};

enum class EHorizontalAnchor
{
	ELeft,
	ECenter,
	ERight
};

enum class EVerticalAnchor
{
	ETop,
	ECenter,
	EBottom
};

struct FPositionData
{
	FCoord Offset = FCoord();
	EHorizontalAnchor HRule = EHorizontalAnchor::ELeft;
	EVerticalAnchor   VRule = EVerticalAnchor::ETop;
	float PivotX = 0;
	float PivotY = 0;
	bool ViewportRelative = false;
};

namespace AvalonUtility
{
	void StringCpy(const char* Source, char* Destination, int Size);
}
