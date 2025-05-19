/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonInputTypes.h"

struct FCoord;

class AvalonInputDevice
{
	/***************************************************************************************
	*  Per Platform Interface
	****************************************************************************************/
public:
	virtual void Initialize() = 0;
	virtual void ProcessInput(FAvalonCurrentInputState& InputState, FCoord& MousePos) = 0;
	/****************************************************************************************/
};

