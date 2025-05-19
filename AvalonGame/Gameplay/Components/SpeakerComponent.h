/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../AvalonActorComponent.h"

#include <string>
#include <vector>
#include <map>

struct FDialogueTree
{
    std::string mFilename;
    std::string mCurrentBranch;
};

/***************************************************************************************
*  Speaker Component - Allows an Actor to 'Speak'
*
*       Associates an Actor with 'Dialogue Trees'.
*
*       Registers itself with Dialogue Manager on Load, so Speakers
*       can be accessed by Name
*
****************************************************************************************/
class SpeakerComponent : public IActorComponent
{
public:
    void Load(tinyxml2::XMLElement* Element) override {}

private:
    std::map<std::string, FDialogueTree> mDialogueTrees;
};
