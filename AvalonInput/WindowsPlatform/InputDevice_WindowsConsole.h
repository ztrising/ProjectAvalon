/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../AvalonInputDevice.h"

class InputDevice_WindowsConsole : public AvalonInputDevice
{

	/***************************************************************************************
	*  Per Platform Interface
	****************************************************************************************/
public:
	void Initialize() override;
	void ProcessInput(FAvalonCurrentInputState& InputState, FCoord& MousePos) override;
	/****************************************************************************************/

private:
	void* GetInputHandle();
};
