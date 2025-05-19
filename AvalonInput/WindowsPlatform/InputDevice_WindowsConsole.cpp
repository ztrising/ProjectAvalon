/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "InputDevice_WindowsConsole.h"

#include "Windows.h"
#include "WinCon.h"

#include "../../WindowsPlatformSupport/WindowsPlatformErrorHandling.h"

#include "../../AvalonUtilities/AvalonTypes.h"
#include "../../AvalonViewport/AvalonViewportManager.h"

// TODO: Read input mappings from config files
bool IsKeyMappedToEvent(unsigned short UnicodeChar, EInputEvent& OutInputEvent)
{
	if (UnicodeChar == 'e')
	{
		OutInputEvent = EInputEvent::EQuit;
		return true;
	}

	if (UnicodeChar == 'l')
	{
		OutInputEvent = EInputEvent::ELoadGame;
		return true;
	}

	if (UnicodeChar == 'n')
	{
		OutInputEvent = EInputEvent::ENewGame;
		return true;
	}

	return false;
}

void InputDevice_WindowsConsole::Initialize()
{
	// Disable Selection of Text
	const HANDLE InputConsole = GetInputHandle();

	DWORD InputSettings = ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT;
	if (!SetConsoleMode(InputConsole, InputSettings))
	{
		AvalonWindowsError::HandleWindowsError("Setting Input Mode Failed...");
	}
}

void InputDevice_WindowsConsole::ProcessInput(FAvalonCurrentInputState& InputState, FCoord& MousePos)
{
	const HANDLE InputConsole = GetInputHandle();
	DWORD NumInputEvents;
	if (!GetNumberOfConsoleInputEvents(InputConsole, &NumInputEvents))
	{
		AvalonWindowsError::HandleWindowsError("GetNumberOfConsoleInputEvents");
	}

	if (NumInputEvents > 0)
	{
		PINPUT_RECORD InputRecords = new INPUT_RECORD[NumInputEvents]();
		DWORD NumInputsToRead = 1;
		DWORD OutNumEvents = 0;

		if (!PeekConsoleInput(InputConsole, InputRecords, NumInputEvents, &OutNumEvents))
		{
			AvalonWindowsError::HandleWindowsError("PeekConsoleInput");
		}

		for (unsigned int i = 0; i < OutNumEvents; ++i)
		{
			INPUT_RECORD& Record = InputRecords[i];
			WORD EventType = Record.EventType;
			bool IsKeyDown = Record.Event.KeyEvent.bKeyDown;

			if (EventType == KEY_EVENT)
			{
				unsigned short UnicodeKey = Record.Event.KeyEvent.uChar.UnicodeChar;
				EInputEvent MappedEvent;
				if (IsKeyMappedToEvent(UnicodeKey, MappedEvent))
				{
					unsigned int Index = static_cast<unsigned int>(MappedEvent);
					InputState[Index].SetKeyDown(IsKeyDown);
				}
			}
			else if (EventType == MOUSE_EVENT)
			{
				COORD Pos = Record.Event.MouseEvent.dwMousePosition;
				MousePos.X = Pos.X;
				MousePos.Y = Pos.Y;

				DWORD MouseButtonState = Record.Event.MouseEvent.dwButtonState;
				if (MouseButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
				{
					InputState[static_cast<unsigned int>(EInputEvent::ELeftMouseButton)].SetKeyDown(IsKeyDown);
				}
				else if (MouseButtonState == RIGHTMOST_BUTTON_PRESSED)
				{
					InputState[static_cast<unsigned int>(EInputEvent::ERightMouseButton)].SetKeyDown(IsKeyDown);
				}
			}
			else if (EventType == WINDOW_BUFFER_SIZE_EVENT)
			{

			}
			else if (EventType == MENU_EVENT)
			{

			}
			else if (EventType == FOCUS_EVENT)
			{

			}
		}

		delete[] InputRecords;

		if (!FlushConsoleInputBuffer(InputConsole))
		{
			AvalonWindowsError::HandleWindowsError("FlushConsoleInputBuffer");
		}
	}
}

void* InputDevice_WindowsConsole::GetInputHandle()
{
	return GetStdHandle(STD_INPUT_HANDLE);
	//return AvalonViewportManager::GetViewportManager().GetInputHandle();
}