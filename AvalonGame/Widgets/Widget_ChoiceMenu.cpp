/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_ChoiceMenu.h"
#include "../Gameplay/Actor/AvalonActor.h"
#include "../Gameplay/Actions/ActionManager.h"

Widget_ChoiceMenu::Widget_ChoiceMenu()
{
	mStartVisible = false;

	mValidHUDContexts = EHUDContext::LOCATION | EHUDContext::TRAVELLING;

	mColorData[EActionContext::Inventory]	= AVALON_FG_YELLOW;
	mColorData[EActionContext::Container]	= AVALON_FG_YELLOW;
	mColorData[EActionContext::Shop]		= AVALON_FG_BLUE;
	mColorData[EActionContext::Dialogue]	= AVALON_FG_CYAN;
	mColorData[EActionContext::Travel]		= AVALON_FG_GREEN;
	mColorData[EActionContext::Combat]		= AVALON_FG_RED;
}

Widget_ChoiceMenu::~Widget_ChoiceMenu()
{
	// TODO:
	// Unbind
	// Remove child
}

void Widget_ChoiceMenu::RepositionMenu(unsigned int NumActions)
{
	FCoord StartLocation = FCoord(mPositionData.Offset.X, mCachedYLoc);
	FCoord EndLocation = StartLocation;
	if (NumActions > 0)
	{
		EndLocation.Y -= 1;
	}

	EndLocation.Y -= 2 * NumActions;

	if (NumActions > 2)
	{
		EndLocation.Y--;
		EndLocation.Y--;
	}

	StopAnimations();
	FMoveAnimSettings MoveAnim;
	MoveAnim.mStartLocation = StartLocation;
	MoveAnim.mEndLocation = EndLocation;
	MoveAnim.mLength = 0.25f;
	MoveTo(&MoveAnim);
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_ChoiceMenu::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct("W_ChoiceMenu_Frame.xml");
	SetText("######################################", FCoord(2, 1));

	// TODO:	Create a ItemDesc sublcass OR a slick way to gain
	//			access to dynamically created children...
	mItemDescWidget = AddChild<AvalonWidget>("W_ItemPanel_ItemDesc.xml");
	Get<AvalonWidget>(mItemDescWidget)->Hide();

	ActionManager::ActionEvent::Callback Callback = Widget_ChoiceMenu::OnActionsUpdated;
	ActionManager::Get().BindActionEvent(this, Callback);

	mCachedYLoc = mPositionData.Offset.Y;
}

void Widget_ChoiceMenu::OnGameLoaded()
{
	AvalonWidget::OnGameLoaded();

	RepositionMenu();
}
/****************************************************************************************/

/***************************************************************************************
*  IEventListener
****************************************************************************************/
/*static*/ void Widget_ChoiceMenu::OnActionsUpdated( IEventListener* Listener
												   , const FActionState& ActionState)
{
	Widget_ChoiceMenu* Widget = static_cast<Widget_ChoiceMenu*>(Listener);
	Widget->UpdateActions(ActionState);
}

/*static*/ void Widget_ChoiceMenu::HandleButtonPressed( IEventListener* Listener
													  , const Widget_Button* Source)
{
	Widget_ChoiceMenu* Widget = static_cast<Widget_ChoiceMenu*>(Listener);

	unsigned int NumChoices = Widget->mActions.size();
	for (unsigned int Index = 0; Index < NumChoices; ++Index)
	{
		FActionMenuEntry& Entry = Widget->mActions[Index];
		if (Entry.mButton.get() == Source)
		{
			Entry.mChoice->Execute();
			ActionManager::Get().ClearActionFocus();
			return;
		}
	}
}

void Widget_ChoiceMenu::UpdateActions(const FActionState& ActionState)
{
	PopulateFromChoiceArray(ActionState.mActions);

	HardUnitRef FocusActorRef = ActionState.mFocus.lock();
	AvalonActor* FocusActor = Get<AvalonActor>(FocusActorRef);

	UpdateActionFocus(FocusActor);

	unsigned int NumActions = ActionState.mActions.size();
	RepositionMenu(NumActions);
	ApplyAttributes(mColorData[ActionState.mContext]);
	Show();
}
/****************************************************************************************/

void Widget_ChoiceMenu::PopulateFromChoiceArray(const ActionList& Source)
{
	unsigned int NumEntries = mActions.size();
	for (unsigned int Index = 0; Index < NumEntries; ++Index)
	{
		FActionMenuEntry Entry = mActions[Index];
		RemoveChild<Widget_Button>(Entry.mButton);
	}
	mActions.clear();

	FCoord ButtonPos = FCoord(4, 3);
	FCoord TextPos = FCoord(5, 0);

	unsigned int NumChoices = Source.size();
	for (unsigned int Index = 0; Index < NumChoices; ++Index)
	{
		FActionMenuEntry NewEntry;
		NewEntry.mChoice = Source[Index];
		NewEntry.mButton = AddChild<Widget_Button>("W_ChoiceMenu_ChoiceButton.xml");

		Widget_Button* Button = Get<Widget_Button>(NewEntry.mButton);

		const char* ChoiceText = NewEntry.mChoice->mActionPrompt.c_str();
		FTextSettings TextSettings;
		TextSettings.mOffset = TextPos;
		TextSettings.mExtent = FCoord(32, 2);
		TextSettings.mJustified = ETextJustification::Left;
		Button->SetTextWithSettings(ChoiceText, TextSettings);

		Widget_Button::ButtonEvent::Callback ButtonPressedCallback = Widget_ChoiceMenu::HandleButtonPressed;
		Button->mOnButtonPressed.BindEvent(this, ButtonPressedCallback);

		Button->SetPosition(ButtonPos);
		ButtonPos.Y += 3;

		mActions.push_back(NewEntry);
	}
}

void Widget_ChoiceMenu::UpdateActionFocus(const AvalonActor* FocusActor)
{
	AvalonWidget* DescWidget = Get<AvalonWidget>(mItemDescWidget);

	if (FocusActor != nullptr)
	{
		FTextSettings TextSettings;
		TextSettings.mOffset = FCoord(2, 0);
		TextSettings.mExtent = FCoord(16, 1);
		TextSettings.mJustified = ETextJustification::Left;
		TextSettings.mFillExtent = true;
		DescWidget->SetTextWithSettings(FocusActor->mDisplayName.c_str(), TextSettings);

		TextSettings.mOffset = FCoord(3, 2);
		TextSettings.mExtent = FCoord(37, 4);
		DescWidget->SetTextWithSettings(FocusActor->mDescription.c_str(), TextSettings);
		DescWidget->Show();
	}
	else
	{
		DescWidget->Hide();
	}
}
/****************************************************************************************/
