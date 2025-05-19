/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "DialogueManager.h"
#include "../../AssetLoaders/AssetLoader.h"
#include "../Actions/ActionManager.h"

void DialogueManager::LoadFromSave(const char* SaveFile)
{

}

void DialogueManager::Update()
{
    if (mPendingExitRequest)
    {
        mIsInDialogue = false;
        mPendingExitRequest = false;
      /*  unsigned int NumListeners = mListeners.Num();
        for (unsigned int Index = 0; Index < NumListeners; ++Index)
        {
            mListeners[Index]->OnDialogueClosed();
        }*/

       /// ActionManager::Get().UpdateCurrentActions();
    }
    else if (mPendingRequest)
    {
        FDialogueInfo* NewData = new FDialogueInfo();
        //AssetLoaders::LoadDialogueData(mPendingRequest, NewData);

        if (mCurrentInfo)
        {
            delete mCurrentInfo;
        }

        mCurrentInfo = NewData;
        mIsInDialogue = true;

       /* unsigned int NumListeners = mListeners.Num();
        for (unsigned int Index = 0; Index < NumListeners; ++Index)
        {
            mListeners[Index]->OnDialogueOpened(mCurrentInfo);
        }*/

        delete mPendingRequest;
        mPendingRequest = nullptr;

        //ActionManager::Get().UpdateCurrentActions();
    }
}

void DialogueManager::RequestExitDialogue()
{
    mPendingExitRequest = true;
}

void DialogueManager::RequestDialogue(const FDialogueRequest& DialogueRequest)
{
    if (mPendingRequest == nullptr)
    {
        //mPendingRequest = AssetLoaders::MakeNewDialogueRequest(mCurrentInfo, DialogueRequest);
    }
}

void AvalonListeners::ListenForDialogue(IDialogueListener* Listener)
{
    //DialogueManager::GetDialogueManager().mListeners.Add(Listener);
}

void AvalonListeners::UnlistenForDialogue(IDialogueListener* Listener)
{
    //DialogueManager::GetDialogueManager().mListeners.Remove(Listener);
}

/***************************************************************************************
*  Dialogue Actions
****************************************************************************************/
/*void Effect_DialogueRequest::ExecuteEffect()
{
    DialogueManager::GetDialogueManager().RequestDialogue(mRequest);
}

void Effect_DialogueRequest::Load(tinyxml2::XMLElement* Element)
{
    const char* SpeakerName = "";
    Element->QueryStringAttribute("SpeakerName", &SpeakerName);
    mRequest.mSpeakerName = SpeakerName;

    const char* DTFile = "";
    Element->QueryStringAttribute("Tree", &DTFile);
    mRequest.mDialogueTree = DTFile;

    const char* Branch = "";
    Element->QueryStringAttribute("Branch", &Branch);
    //strncpy_s(DialogueAction->mRequest.mBranchID, AVALON_MAX_TEXT_SML, Branch, AVALON_MAX_TEXT_SML);
}

void Effect_ExitDialogue::ExecuteEffect()
{
    DialogueManager::GetDialogueManager().RequestExitDialogue();
}

void Effect_ExitDialogue::Load(tinyxml2::XMLElement* Element)
{
   // No serialized data here
}*/
/****************************************************************************************/
