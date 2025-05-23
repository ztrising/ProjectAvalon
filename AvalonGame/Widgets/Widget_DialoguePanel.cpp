/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_DialoguePanel.h"
//#include "../Actions/AvalonAction.h"
//#include "../Dialogue/DialogueManager.h"
#include "AvalonHUD.h"
#include "Widget_ChoiceMenu.h"
#include "../../AvalonViewport/FrameBuffer/FrameBuffer.h"

Widget_DialoguePanel::Widget_DialoguePanel()
{
	AvalonListeners::ListenForDialogue(this);
	mStartVisible = false;

	mValidHUDContexts = EHUDContext::TITLESCREEN;
}

Widget_DialoguePanel::~Widget_DialoguePanel()
{
	AvalonListeners::UnlistenForDialogue(this);
}

void Widget_DialoguePanel::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct("W_DialoguePanel_Frame.xml");

	mSpeakerNameWidgetRef = AddChild<AvalonWidget>("W_DialoguePanel_SpeakerName.xml");
	mSpeakerTextWidgetRef = AddChild<AvalonWidget>("W_DialoguePanel_SpeakerText.xml");
}

void Widget_DialoguePanel::OnDialogueOpened(const FDialogueInfo* Dialogue)
{
	PopulateFromDialogue(Dialogue);
	
	if (!IsVisible())
	{
		FBufferAnimSettings OpenAnim = FBufferAnimSettings();
		OpenAnim.mStyle = EAnimationStyle::FadeIn;
		OpenAnim.mDirection = EAnimationDirection::DrizzleDown;
		OpenAnim.mLength = 1.f;
		//PlayAnimation(OpenAnim);
		Show();
	}
}

void Widget_DialoguePanel::OnDialogueClosed()
{
	Hide();
}

void Widget_DialoguePanel::PopulateFromDialogue(const FDialogueInfo* Dialogue)
{
	//mSpeakerName.Get()->SetTextBox(Dialogue->mSpeakerName, FCoord(), FCoord(), true);
	const char* SpeakerName = Dialogue->mRequest.mSpeakerName.c_str();
	const char* SpeakerText = Dialogue->mText.c_str();

	SetTextBox(SpeakerName, FCoord(0, 1), FCoord(mBuffer.GetSize().X, 1), false, false);
	Get<AvalonWidget>(mSpeakerTextWidgetRef)->SetTextBox(SpeakerText, FCoord(), FCoord(), true);

	FBufferAnimSettings Animation = FBufferAnimSettings();
	Animation.mStyle = EAnimationStyle::FadeIn;
	Animation.mDirection = EAnimationDirection::DrizzleDown;
	Animation.mLength = 7.f;
	//mSpeakerText.Get()->PlayAnimation(Animation);
}


