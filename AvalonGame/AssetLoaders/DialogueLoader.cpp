/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "DialogueLoader.h"
//#include "DialogueTypes.h"

#include "AssetLoader.h"
#include "../../ExternalTools/TinyXML/tinyxml2.h"

#include <stdio.h>
#include <string>




void OpenTreeAsset( const char* TreeFile,
					const char* BranchID,
					tinyxml2::XMLDocument& InTreeDocument, 
					tinyxml2::XMLElement*& OutTreeRoot, 
					tinyxml2::XMLElement*& OutBranch)
{
	AssetLoaders::OpenGameDataFile(TreeFile, InTreeDocument);

	tinyxml2::XMLElement* AvalonData = InTreeDocument.FirstChildElement("AvalonData");
	OutTreeRoot = AvalonData->FirstChildElement("DialogueTree");

	OutBranch = OutTreeRoot->FirstChildElement(BranchID);
	if (OutBranch == nullptr)
	{
		OutBranch = OutTreeRoot->FirstChildElement();
	}
}

/*
void AssetLoaders::LoadDialogueActions(const char* Tree, const char* Branch, DynamicArray<AvalonAction*>& OutActions)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* TreeElement = nullptr;
	tinyxml2::XMLElement* BranchElement = nullptr;

	OpenTreeAsset(Tree, Branch, doc, TreeElement, BranchElement);

	AssetLoaders::PopulateActions(Tree, BranchElement, OutActions);
}
*/

/*void AssetLoaders::LoadDialogueData(FDialogueRequest* DialogueRequest, FDialogueInfo* DialogueInfo)
{
	DialogueInfo->mRequest.mDialogueTree = DialogueRequest->mDialogueTree;
	DialogueInfo->mRequest.mBranch = DialogueRequest->mBranch;

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* TreeElement = nullptr;
	tinyxml2::XMLElement* BranchElement = nullptr;

	OpenTreeAsset(DialogueInfo->mRequest.mDialogueTree.c_str(),
				  DialogueInfo->mRequest.mBranch.c_str(),
				  doc, TreeElement, BranchElement);

	// Speaker Name
	const char* SpeakerName = "Speaker Name";
	TreeElement->QueryStringAttribute("Speaker", &SpeakerName);
	DialogueInfo->mRequest.mSpeakerName = SpeakerName;

	// Speaker Text!
	tinyxml2::XMLElement* SpeakerText = BranchElement->FirstChildElement("SpeakerText");
	std::string SpeakerStr = SpeakerText->GetText();

	//strip it of end lines and tabs
	SpeakerStr.erase(remove(SpeakerStr.begin(), SpeakerStr.end(), '\t'), SpeakerStr.end());
	SpeakerStr.erase(remove(SpeakerStr.begin(), SpeakerStr.end(), '\n'), SpeakerStr.end());

	DialogueInfo->mText = SpeakerStr;
}*/

