/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

class AvalonGame
{
public:
    void StartupGame();
    void SetsWantsExit();

    float GetLastFrameTime() { return mLastFrameTime; }

private:
    void GameLoop();
    bool mWantsExit = false;
    float mLastFrameTime = 0.f;
 
//////////////////////////////////////////////////////////////////////////
//  Singleton Implementation
public:
    static AvalonGame& GetGameInstance()
    {
        static AvalonGame GameInstance;
        return GameInstance;
    }

    AvalonGame(AvalonGame const&) = delete;
    void operator=(AvalonGame const&) = delete;

private:
    AvalonGame() {}
//////////////////////////////////////////////////////////////////////////
};

