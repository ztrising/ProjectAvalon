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
    for (auto& EffectRef : mEffects)
    {
		IAvalonEffect* Effect = Get<IAvalonEffect>(EffectRef);
        Effect->ExecuteEffect(mContext);
    }
}

AvalonAction::~AvalonAction()
{

}

#include "../Utility/JournalTypes.h"

void AvalonEffectFactory(HardUnitRef& OutNewEffect, FSaveContext& Context)
{
	// Serializable Effect Factory
	std::string EffectName = Context.GetSaveID();
	if (EffectName == "JournalEntry")
	{
		AvalonMemory::NewUnit<Effect_JournalEntry>(OutNewEffect);
	}
	/*else if (EffectName == "Damage")
	{
		RetValue = new Effect_StatDamage();
	}*/
}

void AvalonAction::Load(FSaveContext& Context)
{
	Context.AllocateChildrenWithFactory(mEffects, AvalonEffectFactory);
}

void AvalonAction::Save(FSaveContext& Context)
{

}
/****************************************************************************************/
