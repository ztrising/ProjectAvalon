/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

/*
#include "AvalonContainer.h"
#include "AvalonItem.h"

#include "../../ExternalTools/TinyXML/tinyxml2.h"

void IContainerBase::LoadPropertyData(tinyxml2::XMLElement* Element)
{
    Element->QueryFloatAttribute("MaxMass", &mMaxMass);
    Element->QueryFloatAttribute("MaxVolume", &mMaxVolume);

    tinyxml2::XMLElement* ChildElement = Element->FirstChildElement();
    while (ChildElement != nullptr)
    {
        FAvalonItemID NewItemID;
        NewItemID.mID = ChildElement->Value();

        const char* Filename = "failed";
        ChildElement->QueryStringAttribute("Filename", &Filename);
        NewItemID.mFilename = Filename;

        FAvalonItem* Item = new FAvalonItem;
        FAvalonItem::ConstrucItemFromID(NewItemID, *Item);

        ChildElement = ChildElement->NextSiblingElement();
    }
}

void IContainerBase::FindVisibleContainers(std::vector<const IContainerBase*>& OutContainers) const
{
    unsigned int NumItems = mItems.Num();
    for (unsigned int Index = 0; Index < NumItems; ++Index)
    {
        FAvalonItem* Item = mItems[Index];
        if (FContainerData* Container = Item->GetContainerData())
        {
            Container->FindVisibleContainers(OutContainers);
        }
    }
}

bool IContainerBase::CanAddItem(const FAvalonItem& Item) const
{
    if (FPhysicalData* PhysicalData = Item.GetItemProperty<FPhysicalData>())
    {
        if (mMaxMass > 0.f)
        {
            float NewMass = PhysicalData->GetMass() + GetMass();
            if (NewMass > mMaxMass)
            {
                return false; // Too heavy
            }
        }

        if (mMaxVolume > 0.f)
        {
            float NewVolume = PhysicalData->GetVolume() + GetVolume();
            if (NewVolume > mMaxVolume)
            {
                return false; // Too big
            }
        }
    }


    if (!mTagFilter.PassesQuery(Item.GetItemTags()))
    {
        return false; // Failed filter query
    }

    return true;
}

bool IContainerBase::TryAddItem(const FAvalonItem& Item)
{

}

bool IContainerBase::IsEmpty() const
{ 
    return true; 
}

float IContainerBase::GetMass() const
{ 
    return 0.f; 
}

float IContainerBase::GetVolume() const
{
    return 0.f;
}

void FEquipmentData::LoadPropertyData(tinyxml2::XMLElement* Element)
{
    //std::vector<std::string> mSlots;
    //std::vector<std::string> mLayers;
}

bool FEquipmentData::CanAddItem(const FAvalonItem& Item) const
{
    if (FEquipableData* EquipableData = Item.GetItemProperty<FEquipableData>())
    {
        for (FAvalonItem* CheckItem : mItems)
        {
            if (FEquipableData* CheckData = CheckItem->GetItemProperty<FEquipableData>())
            {
                if (CheckData->mSlot == EquipableData->mSlot)
                {
                    if (CheckData->mLayer == EquipableData->mLayer)
                    {
                        return false; // Already got something in that slot/layer
                    }
                }
            }
        }

        return IContainerBase::CanAddItem(Item);
    }

    return false;
}

bool FFastenerData::CanAddItem(const FAvalonItem& Item) const
{
    if (!IsEmpty())
    {
        return false; // Only 1 Fastenable at a time?
    }

    if (FFastenableData* Fastenable = Item.GetItemProperty<FFastenableData>())
    {
        return IContainerBase::CanAddItem(Item);
    }

    return false;
}*/
