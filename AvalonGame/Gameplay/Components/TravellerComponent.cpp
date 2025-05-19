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
			LevelActor::MoveActorTo(GetActorOwnerHandle(), LevelActor::mStreamingLevel);

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
			LevelActor::MoveActorTo(GetActorOwnerHandle(), LevelActor::mStreamingLevel);

			mOnTravelBegin.BroadcastEvent(this);

			if (mPath->mDistance == 0.f)
			{
				FinishTravel();
			}
			else
			{
				// If travel isn't immediate, and this is the player
				// we save the player state
				bool IsPlayer = (GetActorOwnerHandle().Get() == PlayerActor::mPlayer);
				if (IsPlayer)
				{
					PlayerActor::SavePlayer();
					ActionManager::Get().ClearActionFocus();
				}

				GameCalendar::Get()->SetWantsAdvanceTime(this, true);
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
			GameCalendar::Get()->SetWantsAdvanceTime(this, false);
		}

		FWorldPos& Destination = mPathDirection ? mPath->mPointB : mPath->mPointA;

		// When the player travels we change which Levels are loaded
		bool IsPlayer = (GetActorOwnerHandle().Get() == PlayerActor::mPlayer);
		if (IsPlayer)
		{
			ActionManager::Get().ClearActionFocus();
			FLocationInfo* NewLocation = MapUtils::GetLocation(Destination, EGeoScale::EGS_Location);
			LevelActor::OpenLevel(LevelActor::mCurrentLevel, NewLocation);
		}

		std::string LevelSaveID = LevelActor::mCurrentLevel->GetSaveID();
		std::string DestinationID = Destination[EGeoScale::EGS_Location];
		if (LevelSaveID == DestinationID)
		{
			LevelActor::MoveActorTo(GetActorOwnerHandle(), LevelActor::mCurrentLevel);
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
		FLocationInfo* PortalLocation = GetActorOwnerHandle().Get<AvalonActor>()->GetLocation();
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

void PortalComponent::GatherActionsFor(const FUnitHandle& Target, ActionList& OutActions)
{
	AvalonActor* TargetActor = Target.Get<AvalonActor>();
	Traveller* TravelComp = TargetActor->GetComponent<Traveller>();
	if (TravelComp)
	{
		AvalonAction* TravelAction = new AvalonAction();
		TravelAction->mActionPrompt = mTravelPrompt;
		TravelAction->AddEffect<Effect_Travel>();

		// TODO: is there a pattern to the context?
		// is there a way we can do this automagically?
		TravelAction->mContext.mSource = GetActorOwnerHandle();
		TravelAction->mContext.mTarget = Target;

		OutActions.push_back(TravelAction);
	}
}

/***************************************************************************************
*  Travel Effects
****************************************************************************************/
void Effect_Travel::ExecuteEffect(FEffectContext& Context)
{
	AvalonActor* TargetActor = Context.mTarget.Get<AvalonActor>();
	Traveller* TravelComp = TargetActor->GetComponent<Traveller>();

	AvalonActor* SourceActor = Context.mSource.Get<AvalonActor>();
	PortalComponent* PortalComp = SourceActor->GetComponent<PortalComponent>();

	TravelComp->RequestTravel(PortalComp->mRequest);
}




