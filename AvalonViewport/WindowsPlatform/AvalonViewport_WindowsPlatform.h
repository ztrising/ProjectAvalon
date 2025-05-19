/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../AvalonViewport.h"
#include "Windows.h"
//#include "processthreadsapi.h" // PROCESS_INFORMATION

class AvalonViewport_WindowsPlatform : public IAvalonViewport
{
	/***************************************************************************************
	*  Per Platform Interface
	****************************************************************************************/
public:
	void InitializeViewport() override;
	bool ResizeViewport(short SizeX, short SizeY) override;
	bool SetFullscreen() override;
	void WriteArea(short PosX, short PosY, short SizeX, short SizeY) override;
	bool SetCursorVisible(bool Visible) override;
	bool FillScreenTest() override;
	bool DrawArea(const FFrameBuffer& FrameBuffer) override;
	FCoord GetViewportSize() const override;
	void CreateLogWindow() override;
	void TeardownViewport() override;
	void* GetInputHandle() override;
	/****************************************************************************************/

private:

	void CreateGameConsole();

	PROCESS_INFORMATION mGameProcessInformation;
	HANDLE hStdInRead, hStdInWrite, hStdOutRead, hStdOutWrite, hStdErrRead, hStdErrWrite;
};
