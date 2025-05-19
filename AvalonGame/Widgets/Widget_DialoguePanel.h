/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include "../Gameplay/Dialogue/DialogueTypes.h"

class Widget_DialoguePanel : public AvalonWidget, public IDialogueListener
{
public:
	Widget_DialoguePanel();
	~Widget_DialoguePanel();

private:
	void PopulateFromDialogue(const FDialogueInfo* Dialogue);

	FUnitHandle mSpeakerName;
	FUnitHandle mSpeakerText;

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	/****************************************************************************************/

	/***************************************************************************************
	*  IDialogueListener
	****************************************************************************************/
public:
	void OnDialogueOpened(const FDialogueInfo* Dialogue) override;
	void OnDialogueClosed() override;
	/****************************************************************************************/
};

