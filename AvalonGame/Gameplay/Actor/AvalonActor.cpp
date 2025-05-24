/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonActor.h"
#include "LevelActor.h"
#include "../Components/ContainerComponent.h"


void AvalonActor::SetOwner(HardUnitRef& InActor)
{
	mOwner = InActor;
}

void AvalonActor::ResetOwner()
{
	mOwner.reset();
}

bool AvalonActor::IsOwnedBy(const AvalonActor* InActor)
{
	// The actor I'm checking is the owner is not valid!
	if (InActor == nullptr)
	{
		return false;
	}

	// I cannot own myself
	if (InActor == this)
	{
		return false;
	}
	
	// Starting with my owner, keep checking the owner chain
	// until we run out of owners
	AvalonActor* OwnerChainActor = Get<AvalonActor>(mOwner);
	while (OwnerChainActor != nullptr)
	{
		if (OwnerChainActor == InActor)
		{
			return true;
		}

		OwnerChainActor = IAvalonUnit::Get<AvalonActor>(OwnerChainActor->mOwner);
	}

	// Didn't find the In Actor in the ownership chain for this Actor
	return false;
}

void AvalonActor::GatherOwners(ActorList& OutOwners)
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
	HardUnitRef OwnerRef = mOwner.lock();
	while (OwnerRef != nullptr)
	{
		if (OwnerRef.get() == MyLevel && OutOwners.size() > 0)
		{
			return;
		}
		
		AvalonActor* OwnerActor = Get<AvalonActor>(OwnerRef);
		OutOwners.push_back(OwnerActor);

		OwnerRef = OwnerActor->mOwner.lock();
	}
}

LevelActor* AvalonActor::GetLevel()
{
	return Get<LevelActor>(mLevelRef);
}

void AvalonActor::Tick(float DeltaSeconds)
{
	for (HardUnitRef& ComponentRef : mComponents)
	{
		IActorComponent* Component = Get<IActorComponent>(ComponentRef);
		Component->Tick(DeltaSeconds);
	}
}

void AvalonActor::AdvanceTime(long DeltaHours)
{
	for (HardUnitRef& ComponentRef : mComponents)
	{
		IActorComponent* Component = Get<IActorComponent>(ComponentRef);
		Component->AdvanceTime(DeltaHours);
	}
}

void AvalonActor::GatherActionsFor(const AvalonActor* Target, ActionList& OutActions)
{
	for (HardUnitRef& ComponentRef : mComponents)
	{
		IActorComponent* Component = Get<IActorComponent>(ComponentRef);
		if (IActionProvider* Provider = dynamic_cast<IActionProvider*>(Component))
		{
			Provider->GatherActionsFor(Target, OutActions);
		}
	}
}

void AvalonActor::GetItemPouchContainers(ContainerList& OutContainers)
{
	ContainerList Containers;
	GetComponents<IContainer>(Containers);

	for (IContainer* Container : Containers)
	{
		Container->GetItemPouchContainers(OutContainers);
	}
}

void AvalonActor::GatherContainers(ContainerList& OutContainers) const
{
	ContainerList Containers;
	GetComponents<IContainer>(Containers);

	OutContainers.insert( OutContainers.end()
						, Containers.begin(), Containers.end());

	for (IContainer* Container : Containers)
	{
		ContainerList InnerContainers;
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

	HardUnitRef OwnerRef = GetSelfRef();

	// Component Factory - Create and Init Properties!
	auto ComponentFactory_Lambda = [&](HardUnitRef& OutNewComponent, FSaveContext& Context)
	{
		std::string ComponentName = Context.GetSaveID();
		if (ComponentName == "ItemContainer")
		{
			AvalonMemory::NewUnit<ItemContainer>(OutNewComponent);
		}
		else if (ComponentName == "LevelActors")
		{
			AvalonMemory::NewUnit<LevelContainer>(OutNewComponent);
		}
		else if (ComponentName == "Lootable")
		{
			AvalonMemory::NewUnit<Lootable>(OutNewComponent);
		}
		else if (ComponentName == "Looter")
		{
			AvalonMemory::NewUnit<LooterComponent>(OutNewComponent);
		}
		else if (ComponentName == "Equipable")
		{
			AvalonMemory::NewUnit<Equipable>(OutNewComponent);
		}
		else if (ComponentName == "Equipment")
		{
			AvalonMemory::NewUnit<Equipment>(OutNewComponent);
		}
		else if (ComponentName == "Traveller")
		{
			AvalonMemory::NewUnit<Traveller>(OutNewComponent);
		}
		else if (ComponentName == "Portal")
		{
			AvalonMemory::NewUnit<PortalComponent>(OutNewComponent);
		}
		else if (ComponentName == "Stats")
		{
			AvalonMemory::NewUnit<StatsComponent>(OutNewComponent);
		}
		else if (ComponentName == "Meditator")
		{
			AvalonMemory::NewUnit<Meditator>(OutNewComponent);
		}

		IActorComponent* Component = Get<IActorComponent>(OutNewComponent);
		if (Component != nullptr)
		{
			Component->SetActorOwner(OwnerRef);
		}
	};

	Context.AllocateChildrenWithFactory(mComponents, ComponentFactory_Lambda);
}

void AvalonActor::Save(FSaveContext& Context)
{
	mTags.Save("Tags", Context);

	Context.Save("DisplayName", mDisplayName);
	Context.Save("Description", mDescription);

	for (auto& ComponentRef : mComponents)
	{
		IActorComponent* Component = Get<IActorComponent>(ComponentRef);
		ISaveable::Save(Component, Context);
	}
}
