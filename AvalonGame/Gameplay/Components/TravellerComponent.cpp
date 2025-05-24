/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "TravellerComponent.h"

#include "../Actor/AvalonActor.h"
#include "../GameTime/GameCalendar.h"

#include "../Utility/Saveable.h"
#include "../Actor/LevelActor.h"
#include "../Actor/PlayerActor.h"

#include "../Actions/ActionManager.h"
#include "../AvalonGameState.h"

/***************************************************************************************
*  Traveller Component
****************************************************************************************/
void Traveller::Load(FSaveContext& Context)
{
	// Expected Layout: (While travelling)
	/*
		<Traveller	PathID="Archway" 
					Scale="Location" 
					Start="Kaedwen.KaerMorhen.GreatHall"
					Progress="0.25"/>
	*/

	// Expected Layout: (While not travelling)
	/*
		<Traveller />
	*/

	mPathDef.Load(Context);

	if (mPathDef.mPathID != "")
	{
		if (mPath = MapUtils::GetPath(mPathDef))
		{
			mPathDirection = (mPathDef.mStartPos == mPath->mPointA);
			AvalonActor* TravellerActor = GetActorOwner();
			HardUnitRef TravellerRef = TravellerActor->GetSelfRef();
			AvalonGameState::SetActorTravelling(TravellerRef);

			Context.Load("Progress", mProgress);
		}
	}
}

void Traveller::Save(FSaveContext& Context)
{
	if (mPath)
	{
		mPathDef.Save(Context);
		Context.Save("Progress", mProgress);
	}
}

void Traveller::RequestTravel(const FPathRequest& Request)
{
	if (mPath == nullptr)
	{
		if (mPath = MapUtils::GetPath(Request))
		{
			// Save the request in case we save/load during travel
			mPathDef = Request;

			mPathDirection = (Request.mStartPos == mPath->mPointA);

			AvalonActor* TravellingActor = GetActorOwner();
			HardUnitRef TravellingActorRef = TravellingActor->GetSelfRef();
			AvalonGameState::SetActorTravelling(TravellingActorRef);

			mOnTravelBegin.BroadcastEvent(this);

			if (mPath->mDistance == 0.f)
			{
				FinishTravel();
			}
			else
			{
				// If travel isn't immediate, and this is the player
				// we save the player state
				AvalonActor* PlayerActor = AvalonGameState::GetPlayerActor();
				bool IsPlayer = (TravellingActor == PlayerActor);
				if (IsPlayer)
				{
					PlayerActor::SavePlayer();
					ActionManager::Get().ClearActionFocus();
				}

				GameCalendar::SetWantsAdvanceTime(this, true);
			}
		}
	}
}

void Traveller::FinishTravel()
{
	if (mPath)
	{
		if (mPath->mDistance != 0.f)
		{
			GameCalendar::SetWantsAdvanceTime(this, false);
		}

		FWorldPos& Destination = mPathDirection ? mPath->mPointB : mPath->mPointA;

		// When the player travels we change which Levels are loaded
		AvalonActor* TravellingActor = GetActorOwner();
		AvalonActor* PlayerActor = AvalonGameState::GetPlayerActor();

		bool IsPlayer = (TravellingActor == PlayerActor);
		if (IsPlayer)
		{
			ActionManager::Get().ClearActionFocus();
			FLocationInfo* NewLocation = MapUtils::GetLocation(Destination, EGeoScale::EGS_Location);
			AvalonGameState::OpenCurrentLevelAt(NewLocation);
		}

		LevelActor* CurrentLevel = AvalonGameState::GetCurrentLevel();
		std::string LevelSaveID = CurrentLevel->GetSaveID();
		std::string DestinationID = Destination[EGeoScale::EGS_Location];
		if (LevelSaveID == DestinationID)
		{
			HardUnitRef TravellerRef = TravellingActor->GetSelfRef();
			LevelActor::MoveActorTo(TravellerRef, CurrentLevel);
		}
		else
		{
			// TODO:
			// Serialize the NPC into the unloaded level??
		}

		// TODO:  Move the player saves on begin and end travel into the Player Actor class?
		if (IsPlayer)
		{
			PlayerActor::SavePlayer();
		}

		mPath = nullptr;
		mPathDef.mPathID = "";
		mProgress = 0.f;

		mOnTravelEnd.BroadcastEvent(this);
	}
}

void Traveller::AdvanceTime(long DeltaHours)
{
	if (mPath)
	{
		float Distance = mProgress * mPath->mDistance;

		float TravelSpeed = 5.f; // 5 km walking, 20 km horseback
		Distance += (DeltaHours * TravelSpeed);

		mProgress = Distance / mPath->mDistance;

		if (mProgress >= 1.f)
		{
			mProgress = 1.f;
			FinishTravel();
		}
	}
}

/***************************************************************************************
*   Portal Component - Connections to other Locations
****************************************************************************************/
void PortalComponent::Load(FSaveContext& Context)
{
	mRequest.Load(Context);

	if (mRequest.mStartPos.IsValid() == false)
	{
		// No start position given, use position of portal
		AvalonActor* OwnerActor = GetActorOwner();
		FLocationInfo* PortalLocation = OwnerActor->GetLocation();
		mRequest.mStartPos = PortalLocation->mPosition;
		mRequest.mGeoScale = PortalLocation->mGeoScale;
	}

	Context.Load("Prompt", mTravelPrompt);
}

void PortalComponent::Save(FSaveContext& Context)
{
	mRequest.Save(Context);
	Context.Save("Prompt", mTravelPrompt);
}

void PortalComponent::GatherActionsFor(const AvalonActor* Target, ActionList& OutActions)
{
	Traveller* TravelComp = Target->GetComponent<Traveller>();
	if (TravelComp)
	{
		HardUnitRef NewActionRef;
		AvalonAction* TravelAction = AvalonAction::NewAction(NewActionRef);
		TravelAction->mActionPrompt = mTravelPrompt;
		TravelAction->AddEffect<Effect_Travel>();

		// TODO: is there a pattern to the context?
		// is there a way we can do this automagically?
		HardUnitRef PortalActorRef = GetActorOwner()->GetSelfRef();
		TravelAction->mContext.mSource = PortalActorRef;
		TravelAction->mContext.mTarget = Target->GetSelfRef();

		OutActions.push_back(NewActionRef);
	}
}

/***************************************************************************************
*  Travel Effects
****************************************************************************************/
void Effect_Travel::ExecuteEffect(FEffectContext& Context)
{
	AvalonActor* TargetActor = AvalonActor::Get<AvalonActor>(Context.mTarget);
	if (TargetActor)
	{
		Traveller* TravelComp = TargetActor->GetComponent<Traveller>();
		AvalonActor* SourceActor = AvalonActor::Get<AvalonActor>(Context.mSource);
		PortalComponent* PortalComp = SourceActor->GetComponent<PortalComponent>();

		TravelComp->RequestTravel(PortalComp->mRequest);
	}
}




