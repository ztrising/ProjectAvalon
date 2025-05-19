/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonActor.h"
#include "LevelActor.h"


void AvalonActor::SetOwner(const FUnitHandle& InActorHandle)
{
	mOwnerHandle = InActorHandle;
}

void AvalonActor::ResetOwner()
{
	mOwnerHandle.Reset();
}

bool AvalonActor::IsOwnedBy(const FUnitHandle& InActorHandle)
{
	// The actor I'm checking is the owner is not valid!
	if (!InActorHandle.IsValid())
	{
		return false;
	}

	// I cannot own myself
	if (InActorHandle == GetSelfHandle())
	{
		return false;
	}
	
	// Starting with my owner, keep checking the owner chain
	// until we run out of owners
	const AvalonActor* InActor = InActorHandle.Get<AvalonActor>();
	FUnitHandle& OwnerChainHandle = mOwnerHandle;
	while (OwnerChainHandle.IsValid())
	{
		const AvalonActor* OwnerChainActor = OwnerChainHandle.Get<AvalonActor>();
		if (OwnerChainActor == InActor)
		{
			return true;
		}

		OwnerChainHandle = OwnerChainActor->mOwnerHandle;
	}

	// Didn't find the In Actor in the ownership chain for this Actor
	return false;
}

void AvalonActor::GatherOwners(std::vector<FUnitHandle>& OutOwners)
{
	// Levels are allowed to "directly" own actors but aren't allowed
	// to be "grandparents".
	
	// So things directly placed in the level are owned by the level
	// but things INSIDE things in the level are owned only by
	// the chain of things they are inside

	// This is so, when something that is directly placed in the level
	// is placed elsewhere, the level loses ownership and we are
	// notified to update the "location panel" display

	const LevelActor* MyLevel = GetLevel();
	FUnitHandle OwnerChainHandle = mOwnerHandle;
	while (OwnerChainHandle.IsValid())
	{
		const AvalonActor* OwnerChainActor = OwnerChainHandle.Get<AvalonActor>();
		if (OwnerChainActor == MyLevel && OutOwners.size() > 0)
		{
			return;
		}
		
		OutOwners.push_back(OwnerChainHandle);
		OwnerChainHandle = OwnerChainActor->mOwnerHandle;
	}
}

LevelActor* AvalonActor::GetLevel()
{
	return mLevel.Get<LevelActor>();
}

void AvalonActor::Tick(float DeltaSeconds)
{
	for (IActorComponent* Component : mComponents)
	{
		Component->Tick(DeltaSeconds);
	}
}

void AvalonActor::AdvanceTime(long DeltaHours)
{
	for (IActorComponent* Component : mComponents)
	{
		Component->AdvanceTime(DeltaHours);
	}
}

#include "../Components/ContainerComponent.h"

void AvalonActor::GatherActionsFor(const FUnitHandle& Target, ActionList& OutActions)
{
	for (IActorComponent* Component : mComponents)
	{
		if (IActionProvider* Provider = dynamic_cast<IActionProvider*>(Component))
		{
			Provider->GatherActionsFor(Target, OutActions);
		}
	}
}

void AvalonActor::GetItemPouchContainers(std::vector<IContainer*>& OutContainers)
{
	std::vector<IContainer*> Containers;
	GetComponents<IContainer>(Containers);

	for (IContainer* Container : Containers)
	{
		Container->GetItemPouchContainers(OutContainers);
	}
}

void AvalonActor::GatherContainers(std::vector<IContainer*>& OutContainers)
{
	std::vector<IContainer*> Containers;
	GetComponents<IContainer>(Containers);

	OutContainers.insert( OutContainers.end()
						, Containers.begin(), Containers.end());

	for (IContainer* Container : Containers)
	{
		std::vector<IContainer*> InnerContainers;
		Container->GetContainersWithin(InnerContainers);

		OutContainers.insert( OutContainers.end()
							, InnerContainers.begin(), InnerContainers.end());
	}

}

#include "../Components/StatsComponent.h"
#include "../Components/LootableComponent.h"
#include "../Components/TravellerComponent.h"
#include "../Components/Meditator.h"

void AvalonActor::Load(FSaveContext& Context)
{
	mTags.Load("Tags", Context);

	Context.Load("DisplayName", mDisplayName);
	Context.Load("Description", mDescription);

	//strip it of end lines and tabs
	mDescription.erase(remove(mDescription.begin(), mDescription.end(), '\t'), mDescription.end());
	mDescription.erase(remove(mDescription.begin(), mDescription.end(), '\n'), mDescription.end());

	// Component Factory - Create and Init Properties!
	auto ComponentFactory_Lambda = [&](FSaveContext& Context)
	{
		IActorComponent* RetValue = nullptr;

		std::string ComponentName = Context.GetSaveID();
		if (ComponentName == "ItemContainer")
		{
			RetValue = new ItemContainer();
		}
		else if (ComponentName == "LevelActors")
		{
			RetValue = new LevelContainer();
		}
		else if (ComponentName == "Lootable")
		{
			RetValue = new Lootable();
		}
		else if (ComponentName == "Looter")
		{
			RetValue = new LooterComponent();
		}
		else if (ComponentName == "Equipable")
		{
			RetValue = new Equipable();
		}
		else if (ComponentName == "Equipment")
		{
			RetValue = new Equipment();
		}
		else if (ComponentName == "Traveller")
		{
			RetValue = new Traveller();
		}
		else if (ComponentName == "Portal")
		{
			RetValue = new PortalComponent();
		}
		else if (ComponentName == "Stats")
		{
			RetValue = new StatsComponent();
		}
		else if (ComponentName == "Meditator")
		{
			RetValue = new Meditator();
		}

		if (RetValue != nullptr)
		{
			RetValue->SetActorOwner(GetSelfHandle());
		}

		return RetValue;
	};

	Context.AllocateChildrenWithFactory(mComponents, ComponentFactory_Lambda);
}

void AvalonActor::Save(FSaveContext& Context)
{
	mTags.Save("Tags", Context);

	Context.Save("DisplayName", mDisplayName);
	Context.Save("Description", mDescription);

	for (IActorComponent* Component : mComponents)
	{
		ISaveable::Save(Component, Context);
	}
}
