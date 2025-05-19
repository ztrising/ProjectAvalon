/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonGame.h"

#include "../AvalonInput/AvalonInputManager.h"
#include "../AvalonViewport/AvalonViewportManager.h"
#include "Gameplay/AvalonGameState.h"

#include <windows.h>

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter()
{
	LARGE_INTEGER li;

	QueryPerformanceFrequency(&li);
	PCFreq = double(li.QuadPart);

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}
double GetCounter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	double numerator = double(li.QuadPart - CounterStart);
	if (numerator > 0)
	{
		return numerator / PCFreq;
	}

	return 0;
}

void AvalonGame::StartupGame()
{
	AvalonViewportManager::GetViewportManager().Initialize();
	AvalonInputManager::GetInputManager().Initialize();
	AvalonGameState::GetGameState().Init();

	mWantsExit = false;

	GameLoop();
}

void AvalonGame::GameLoop()
{
	void StartCounter();
	mLastFrameTime = 0;

	while (!mWantsExit)
	{
		// Input
		AvalonInputManager::GetInputManager().ProcessInput();

		// Game Logic
		AvalonGameState::GetGameState().TickGame(mLastFrameTime);

		// Draw Step
		AvalonViewportManager::GetViewportManager().Draw(mLastFrameTime);

		// Garbage Collection
		AvalonMemory::Get().TickGarbageCollection(mLastFrameTime);
		//AvalonMemory::TickGarbageCollection(mLastFrameTime);

		mLastFrameTime = GetCounter();
		StartCounter();
	}
}

void AvalonGame::SetsWantsExit()
{
	mWantsExit = true;
}
