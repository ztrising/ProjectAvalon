/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonAction.h"
#include "ActionManager.h"

#include "../Actor/PlayerActor.h"
#include "../AvalonGameState.h"

/***************************************************************************************
*  Action Manager
****************************************************************************************/
void ActionManager::UpdateCurrentActions()
{
	PopulateActions();

	mOnActionsUpdated.BroadcastEvent(mActionState);
}

void ActionManager::PopulateActions()
{
	mActionState.mActions.clear();

	if (AvalonActor* FocusActor = IAvalonUnit::Get<AvalonActor>(mActionState.mFocus))
	{
		AvalonActor* Player = AvalonGameState::GetPlayerActor();
		FocusActor->GatherActionsFor(Player, mActionState.mActions);
	}
}

void ActionManager::SetActionFocus(HardUnitRef NewFocus)
{
	mActionState.mFocus = NewFocus;
	UpdateCurrentActions();
}

void ActionManager::ClearActionFocus()
{
	mActionState.mFocus.reset();
	UpdateCurrentActions();
}

/***************************************************************************************
*  AvalonAction
****************************************************************************************/
void AvalonAction::Execute()
{
    for (IAvalonEffect* Effect : mEffects)
    {
        Effect->ExecuteEffect(mContext);
    }
}

AvalonAction::~AvalonAction()
{
	for (IAvalonEffect* Effect : mEffects)
    {
        delete Effect;
    }
}

#include "../Utility/JournalTypes.h"

IAvalonEffect* AvalonEffectFactory(FSaveContext& Context)
{
	IAvalonEffect* RetValue = nullptr;

	// Serializable Effect Factory
	std::string EffectName = Context.GetSaveID();
	if (EffectName == "JournalEntry")
	{
		RetValue = new Effect_JournalEntry();
	}
	/*else if (EffectName == "Damage")
	{
		RetValue = new Effect_StatDamage();
	}*/

	return RetValue;
}

void AvalonAction::Load(FSaveContext& Context)
{
	Context.AllocateChildrenWithFactory(mEffects, AvalonEffectFactory);
}

void AvalonAction::Save(FSaveContext& Context)
{

}
/****************************************************************************************/
