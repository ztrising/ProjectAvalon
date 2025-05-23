/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_ToggleButton.h"
#include "../Gameplay/Actions/AvalonAction.h"
#include "../Gameplay/Utility/EventDispatcher.h"

struct FActionMenuEntry
{
	HardUnitRef mButton;
	AvalonAction* mChoice = nullptr;

	bool operator==(const FActionMenuEntry& rhs) const
	{
		return (mButton == rhs.mButton);
	}
};

/***************************************************************************************
*  Widget_ChoiceMenu
* 
*  An array of buttons associated with choices, that runs those choice actions
*  when pressed.
* 
*  How the player makes choices!
* 
*  Priority:  Item Detail > Dialogue > Location
* 
*  Cannot be used during travel!
****************************************************************************************/

class Widget_ChoiceMenu : public AvalonWidget, public IEventListener
{
public:
	Widget_ChoiceMenu();
	~Widget_ChoiceMenu();

	void PopulateFromChoiceArray(const ActionList& Source);
	void UpdateActionFocus(const AvalonActor* FocusActor);

	void UpdateActions(const FActionState& ActionState);
	
private:
	void RepositionMenu(unsigned int NumActions = 0);

	std::vector<FActionMenuEntry> mActions;
	unsigned short mColorData[EActionContext::Count];

	// Item Focus Desc
	HardUnitRef mItemDescWidget;
	short mCachedYLoc;

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	void OnGameLoaded() override;
	/****************************************************************************************/

	/***************************************************************************************
	*  IEventListener
	****************************************************************************************/
public:
	static void OnActionsUpdated( IEventListener* Listener
								, const FActionState& ActionState);

	static void HandleButtonPressed( IEventListener* Listener
								   , const Widget_Button* Source);
	/****************************************************************************************/
};
