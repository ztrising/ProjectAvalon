/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

/*#include "InventoryLoader.h"
#include "ItemTypes.h"
#include "AvalonInventory.h"
#include "../AssetLoaders/AssetLoader.h"

#include "../../ExternalTools/TinyXML/tinyxml2.h"

#include <stdio.h>
#include <string>

// THE NEW STUFF
#include "AvalonItem.h"
#include "AvalonContainer.h"

void AssetLoaders::LoadInventory(AvalonInventory& Inventory)
{
	// TODO: Support multiple item pouches? Maybe?
	FItemContainer& ItemPouch = Inventory.GetContainer(EContainerType::ItemPouch);

	strncpy_s(ItemPouch.mContainerID.mFileName, AVALON_MAX_TEXT_SML, "Cont_Player_Inventory.xml", AVALON_MAX_TEXT_SML);
	strncpy_s(ItemPouch.mContainerID.mID, AVALON_MAX_TEXT_SML, "ItemPouch", AVALON_MAX_TEXT_SML);

	AssetLoaders::LoadContainerFromFile(ItemPouch.mContainerID, ItemPouch);
}

AvalonAction::EContext AssetLoaders::LoadItemActions(AvalonInventory& Inventory, DynamicArray<FAvalonAction*>& OutActions)
{
	AvalonAction::EContext Context = AvalonAction::EContext::Count;

	FInventoryItem* Focus			= Inventory.GetItemFocus();
	FItemContainer* ContainerFocus	= Inventory.GetContainerFocus();

	auto MoveActionLambda = [&](FItemContainer& Source, FItemContainer& Destination, const char* MoveText)
	{
		if (Destination.HasCapacity())
		{
			FAvalonAction* NewAction = new FAvalonAction();
			strncpy_s(NewAction->mActionText, AVALON_MAX_TEXT_MED_SML, MoveText, AVALON_MAX_TEXT_MED_SML);
			strncat_s(NewAction->mActionText, AVALON_MAX_TEXT_MED_SML, Destination.mContainerName, AVALON_MAX_TEXT_MED_SML);

			Effect_MoveItem* MoveEffect = new Effect_MoveItem();
			MoveEffect->mItem = Focus;
			MoveEffect->mSource = &Source;
			MoveEffect->mDestination = &Destination;

			NewAction->mEffects.Add(MoveEffect);
			OutActions.Add(NewAction);
		}
	};

	auto ItemActionsLambda = [&]()
	{
		tinyxml2::XMLDocument doc;
		AssetLoaders::OpenGameDataFile(Focus->mItemFileName, doc);

		tinyxml2::XMLElement* AvalonData = doc.FirstChildElement("AvalonData");
		tinyxml2::XMLElement* ItemData = AvalonData->FirstChildElement("AvalonItem");

		AssetLoaders::PopulateActions(Focus->mItemFileName, ItemData, OutActions);
	};

	if (Focus != nullptr)
	{
		EContainerType FocusType = ContainerFocus->mContainerID.mType;
		switch (FocusType)
		{
			case EContainerType::Player: // I'm looking at my own inventory
			{
				if (Inventory.IsTrading())
				{
					// Sell actions?
				}
				else if (Inventory.IsLooting())
				{
					// Move action, i.e. 'putting' your things in world containers
					FItemContainer* Destination = Inventory.GetLootingContainer();
					if (Destination)
					{
						MoveActionLambda(*ContainerFocus, *Destination, "Move to ");
					}

					Context = AvalonAction::EContext::Container;
				}

				// regular item actions
				ItemActionsLambda();

			}
				break;
			case EContainerType::Trade:// I'm looking at items to buy
			{
				// Buy actions
				FItemContainer* Destination = Inventory.GetPlayerContainerWithVacancy(Focus->mItemTypesMask);
				if (Destination)
				{
					FAvalonAction* NewAction = new FAvalonAction();
					snprintf(NewAction->mActionText, AVALON_MAX_TEXT_MED_SML, "Buy %s (%dG)", Focus->mItemName, Focus->mBaseCost);

					Effect_MoveItem* MoveEffect = new Effect_MoveItem();
					MoveEffect->mItem = Focus;
					MoveEffect->mSource = Inventory.GetTradeInventory();
					MoveEffect->mDestination = Destination;

					NewAction->mEffects.Add(MoveEffect);
					OutActions.Add(NewAction);
				}
			}
				break;
			case EContainerType::World:// I'm rifling through a container in the world
			{
				// Move action, i.e. 'putting' your things in your inventory
				FItemContainer* Destination = Inventory.GetPlayerContainerWithVacancy(Focus->mItemTypesMask);
				if (Destination)
				{
					MoveActionLambda(*ContainerFocus, *Destination, "Move to ");
				}

				Context = AvalonAction::EContext::Container;
				
				// regular actions, do we need to limit regular actions from looting?
				ItemActionsLambda();
			}
				break;
			case EContainerType::Transient:
			default:
				break;
		}
	}

	// Always have the close container option if its open
	if (Inventory.IsLooting())
	{
		Context = AvalonAction::EContext::Container;

		FAvalonAction* NewAction = new FAvalonAction();
		strncpy_s(NewAction->mActionText, AVALON_MAX_TEXT_MED_SML, "Close Container", AVALON_MAX_TEXT_MED_SML);
		
		// TODO, container name
		//strncat_s(NewAction->mActionText, AVALON_MAX_TEXT_MED_SML, Container.mContainerName, AVALON_MAX_TEXT_MED_SML);

		Effect_CloseContainer* CloseEffect = new Effect_CloseContainer();

		NewAction->mEffects.Add(CloseEffect);
		OutActions.Add(NewAction);
	}

	// Always have the stop trade option if currently trading
	if (Inventory.IsTrading())
	{
		Context = AvalonAction::EContext::Shop;

		FAvalonAction* NewAction = new FAvalonAction();
		snprintf(NewAction->mActionText, AVALON_MAX_TEXT_MED_SML, "Stop Trade");

		Effect_StopTrade* Stop = new Effect_StopTrade();
		NewAction->mEffects.Add(Stop);

		OutActions.Add(NewAction);
	}

	return Context;
}

void AssetLoaders::LoadItemFromFile(const char* ItemFile, FInventoryItem& OutItem)
{
	// Leaving this here in case there is a situation where this is needed
	// as it is, we fill this out outside of this function
	//strncpy_s(OutItem.mItemFileName, AVALON_MAX_TEXT_SML, ItemFile, AVALON_MAX_TEXT_SML);

	tinyxml2::XMLDocument doc;
	AssetLoaders::OpenGameDataFile(ItemFile, doc);

	tinyxml2::XMLElement* AvalonData = doc.FirstChildElement("AvalonData");
	tinyxml2::XMLElement* ItemData = AvalonData->FirstChildElement("AvalonItem");

	const char* ItemName = "failed";
	ItemData->QueryStringAttribute("ItemName", &ItemName);
	strncpy_s(OutItem.mItemName, AVALON_MAX_TEXT_MED_SML, ItemName, AVALON_MAX_TEXT_MED_SML);

	tinyxml2::XMLElement* DescElement = ItemData->FirstChildElement("ItemDesc");
	std::string DescText = DescElement->GetText();

	//strip it of end lines and tabs
	DescText.erase(remove(DescText.begin(), DescText.end(), '\t'), DescText.end());
	DescText.erase(remove(DescText.begin(), DescText.end(), '\n'), DescText.end());

	//size it down
	DescText.resize(AVALON_MAX_TEXT_LRG);

	strncpy_s(OutItem.mItemDescription, AVALON_MAX_TEXT_LRG, DescText.c_str(), AVALON_MAX_TEXT_LRG);
}

AvalonItems::EEquipSlot AssetLoaders::GetSlotFromString(const char* SlotString)
{
	if (strcmp(SlotString, "SteelSword") == 0)
	{
		return AvalonItems::EEquipSlot::ESteelSword;
	}
	else if (strcmp(SlotString, "SilverSword") == 0)
	{
		return AvalonItems::EEquipSlot::ESilverSword;
	}
	else if (strcmp(SlotString, "Armor") == 0)
	{
		return AvalonItems::EEquipSlot::EArmor;
	}

	return AvalonItems::EEquipSlot::EArmor;
}

void AssetLoaders::LoadContainerFromFile(const FContainerID& ContainerID, FItemContainer& OutContainer)
{
	OutContainer.ClearContainer();

	// First we create the item instance, and fill out their serialized data from the 
	{
		tinyxml2::XMLDocument doc;
		AssetLoaders::OpenGameDataFile(ContainerID.mFileName, doc);

		tinyxml2::XMLElement* Data = doc.FirstChildElement("AvalonData");
		tinyxml2::XMLElement* AllContainers = Data->FirstChildElement("Containers");
		tinyxml2::XMLElement* Container = AllContainers->FirstChildElement(ContainerID.mID);

		const char* ContainerName = "failed";
		Container->QueryStringAttribute("DisplayName", &ContainerName);
		strncpy_s(OutContainer.mContainerName, AVALON_MAX_TEXT_MED_SML, ContainerName, AVALON_MAX_TEXT_MED_SML);

		strncpy_s(OutContainer.mContainerID.mFileName, AVALON_MAX_TEXT_SML, ContainerID.mFileName, AVALON_MAX_TEXT_SML);
		strncpy_s(OutContainer.mContainerID.mID, AVALON_MAX_TEXT_SML, ContainerID.mID, AVALON_MAX_TEXT_SML);

		OutContainer.mMaxCapacity = -1;
		Container->QueryIntAttribute("MaxCapacity", &OutContainer.mMaxCapacity);

		tinyxml2::XMLElement* ItemElement = Container->FirstChildElement();
		while (ItemElement != nullptr)
		{
			FInventoryItem* NewItem = new FInventoryItem();
			OutContainer.mItems.Add(NewItem);

			const char* ItemFileName = "failed";
			ItemElement->QueryStringAttribute("FileName", &ItemFileName);
			strncpy_s(NewItem->mItemFileName, AVALON_MAX_TEXT_SML, ItemFileName, AVALON_MAX_TEXT_SML);

			ItemElement = ItemElement->NextSiblingElement();
		}
	}

	// then for each item we load that file and populate the item info
	unsigned int NumItems = OutContainer.mItems.Num();
	for (unsigned int Index = 0; Index < NumItems; ++Index)
	{
		FInventoryItem* Item = OutContainer.mItems[Index];
		LoadItemFromFile(Item->mItemFileName, *Item);
	}
}

void AssetLoaders::SaveContainer(const FItemContainer& InContainer)
{
	const FContainerID& ContainerID = InContainer.mContainerID;

	tinyxml2::XMLDocument doc;
	AssetLoaders::OpenGameDataFile(ContainerID.mFileName, doc);

	tinyxml2::XMLElement* Data = doc.FirstChildElement("AvalonData");
	tinyxml2::XMLElement* AllContainers = Data->FirstChildElement("Containers");

	tinyxml2::XMLElement* Container = AllContainers->FirstChildElement(ContainerID.mID);

	// This could also be how we create containers!
	if (Container == nullptr)
	{
		Container = AllContainers->InsertNewChildElement(ContainerID.mID);
		Container->SetAttribute("DisplayName", InContainer.mContainerName);
	}

	// For simplicity, maybe we should delete everything and just 
	// re-add everything...
	Container->DeleteChildren();

	// So for now, let's assume the container data is empty...
	unsigned int NumItems = InContainer.mItems.Num();
	for(unsigned int Index = 0; Index < NumItems; ++Index)
	{
		const FInventoryItem* Item = InContainer.mItems[Index];
		tinyxml2::XMLElement* NewElement = Container->InsertNewChildElement("Item");
		NewElement->SetAttribute("FileName", Item->mItemFileName);
	}

	AssetLoaders::SaveGameDataFile(InContainer.mContainerID.mFileName, doc);
}*/

