/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonInputManager.h"
#include "AvalonInputDevice.h"

// Windows Platform Input Processor!
#include "WindowsPlatform/InputDevice_WindowsConsole.h"

AvalonInputManager::AvalonInputManager()
{
	// Windows Console Input!
	mInputDevice = new InputDevice_WindowsConsole();
}

void AvalonInputManager::Initialize()
{
	if (mInputDevice)
	{
		mInputDevice->Initialize();
	}
}

void AvalonInputManager::ProcessInput()
{
	bool MousePosChanged = false;
	if (mInputDevice)
	{
		FCoord OldPos = FCoord();
		OldPos.X = mCurrentMousePosition.X;
		OldPos.Y = mCurrentMousePosition.Y;

		mInputDevice->ProcessInput(mInputState, mCurrentMousePosition);

		if (OldPos.X != mCurrentMousePosition.X || OldPos.Y != mCurrentMousePosition.Y)
		{
			MousePosChanged = true;
		}
	}

	for (unsigned int InputEvent = 0; InputEvent < AVALON_NUM_INPUT_KEYS; ++InputEvent)
	{
		FAvalonInputEventState& KeyState = mInputState[InputEvent];
		if (KeyState.HasChangedThisFrame())
		{
			EInputEvent Event = static_cast<EInputEvent>(InputEvent);
			bool KeyDown = KeyState.IsKeyDown();
			bool Handled = false;

			FInputKeyEventParams Params;
			Params.mEvent	= Event;
			Params.mKeyDown = KeyState.IsKeyDown();

			mOnKeyStateChange.BroadcastEvent(Params);

			KeyState.Processed();
		}
	}

	if (MousePosChanged)
	{
		FInputMouseEventParams Params;
		Params.mMousePosition = mCurrentMousePosition;
		
		mOnMouseStateChange.BroadcastEvent(Params);
	}
}

bool AvalonInputManager::IsKeyDown(EInputEvent Event)
{
	return mInputState[static_cast<unsigned int>(Event)].IsKeyDown();
}
