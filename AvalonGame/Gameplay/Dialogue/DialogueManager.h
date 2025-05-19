/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "DialogueTypes.h"
#include "../../AssetLoaders/DialogueLoader.h"
#include <vector>

#include <map>

class SpeakerComponent;

class DialogueManager
{
public:

    // Maybe make a Game Manager interface?
    void LoadFromSave(const char* SaveFile);
    void Update();

    /***************************************************************************************
    *  Speakers!  Currently Loaded Actors that you can Dialogue with
    ****************************************************************************************/
public:
    void RegisterSpeaker(SpeakerComponent* Speaker) {}
    void UnregisterSpeaker(SpeakerComponent* Speaker) {}

private:
   // std::map<std::string, SpeakerComponent> mLoadedSpeakers;
    /****************************************************************************************/

    //
    void RequestExitDialogue();
    void RequestDialogue(const FDialogueRequest& DialogueRequest);

    FDialogueInfo* GetCurrentDialogueInfo() { return mCurrentInfo; }
    bool IsInDialogue() { return mIsInDialogue; };

private:
    bool mIsInDialogue = false;
    bool mPendingExitRequest = false;
    FDialogueRequest* mPendingRequest = nullptr;
    FDialogueInfo* mCurrentInfo = nullptr;

    //friend void AssetLoaders::LoadDialogueData(FDialogueRequest* DialogueRequest, FDialogueInfo* DialogueInfo);

    /***************************************************************************************
    *  Dispatching Dialogue Events
    ****************************************************************************************/
    friend void AvalonListeners::ListenForDialogue(IDialogueListener* Listener);
    friend void AvalonListeners::UnlistenForDialogue(IDialogueListener* Listener);

    std::vector<IDialogueListener*> mListeners;
    /****************************************************************************************/

    //////////////////////////////////////////////////////////////////////////
    //  Singleton Implementation
public:
    static DialogueManager& GetDialogueManager()
    {
        static DialogueManager Manager;
        return Manager;
    }

    DialogueManager(DialogueManager const&) = delete;
    void operator=(DialogueManager const&) = delete;

private:
    DialogueManager() {}
    //////////////////////////////////////////////////////////////////////////
};

