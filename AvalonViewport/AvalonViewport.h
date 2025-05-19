/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "FrameBuffer/FrameBuffer.h"

class IAvalonViewport
{
	/***************************************************************************************
	*  Per Platform Interface
	****************************************************************************************/
public:
	virtual void InitializeViewport() = 0;
	virtual bool ResizeViewport(short SizeX, short SizeY) = 0;
	virtual bool SetFullscreen() = 0;
	virtual void WriteArea(short PosX, short PosY, short SizeX, short SizeY) = 0;
	virtual bool SetCursorVisible(bool Visible) = 0;
	virtual bool FillScreenTest() = 0;
	virtual bool DrawArea(const FFrameBuffer& FrameBuffer) = 0;
	virtual FCoord GetViewportSize() const = 0;
	virtual void CreateLogWindow() = 0;
	virtual void TeardownViewport() = 0;
	virtual void* GetInputHandle() = 0;
	/****************************************************************************************/
};
