/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../Actions/AvalonAction.h"
#include <string>

struct FAvalonAction;

struct FDialogueRequest
{
    std::string mSpeakerName;
    std::string mDialogueTree;
    std::string mBranch;
};

struct FDialogueInfo
{
    FDialogueRequest mRequest;
    std::string mText;
};

class IDialogueListener
{
    /***************************************************************************************
    *  IDialogueListener
    ****************************************************************************************/
public:
    virtual void OnDialogueOpened(const FDialogueInfo* Dialogue) = 0;
    virtual void OnDialogueClosed() = 0;
    /****************************************************************************************/
};

namespace AvalonListeners
{
    void ListenForDialogue(IDialogueListener* Listener);
    void UnlistenForDialogue(IDialogueListener* Listener);
}

/***************************************************************************************
*  Dialogue Actions
****************************************************************************************/
/*struct Effect_DialogueRequest : public IAvalonEffect
{
    void Load(tinyxml2::XMLElement* Element) override;
    void Save(tinyxml2::XMLElement* Element) override;

    void ExecuteEffect(FEffectContext& Context) override;
    FDialogueRequest mRequest;
};

struct Effect_ExitDialogue : public IAvalonEffect
{
    void ExecuteEffect(FEffectContext& Context) override;
};*/
/****************************************************************************************/
