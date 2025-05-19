/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "InputListener.h"
#include "AvalonInputManager.h"

void IInputListener::StartListeningForInput(IInputListener* Listener)
{
	if (Listener->mWantsInput && !Listener->mIsListeningForInput)
	{
		//AvalonInputManager::GetInputManager().RegisterInputListener(Listener);
		Listener->mIsListeningForInput = true;
	}
}

void IInputListener::StopListeningForInput(IInputListener* Listener)
{
	if (Listener->mIsListeningForInput)
	{
		//AvalonInputManager::GetInputManager().UnregisterInputListener(Listener);
		Listener->mIsListeningForInput = false;
	}
}
