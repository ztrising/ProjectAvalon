/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

/*

#include "AvalonItemProperty.h"
#include <vector>

struct FAvalonItem;
struct FContainerData;

struct IContainerBase : public IAvalonItemProperty
{
    virtual void LoadPropertyData(tinyxml2::XMLElement* Element) override;

    void FindVisibleContainers(std::vector<const IContainerBase*>& OutContainers) const;

    virtual bool CanAddItem(const FAvalonItem& Item) const;
    bool TryAddItem(const FAvalonItem& Item);

    bool IsEmpty() const;
    float GetMass() const;
    float GetVolume() const;

protected:
    float mMaxMass = -1.f;
    float mMaxVolume = -1.f;
    //FAvalonTagQuery mTagFilter;

    std::vector<FAvalonItem*> mItems;
};

struct FContainerData : public IContainerBase
{
   // Just in case we want something specific
    // to 'general' containers not shared
    // by the other types
};

// Slots for things you wear
struct FEquipmentData : public IContainerBase
{
public:
    void LoadPropertyData(tinyxml2::XMLElement* Element) override;

    bool CanAddItem(const FAvalonItem& Object) const override;

    std::vector<std::string> mSlots;
    std::vector<std::string> mLayers;
};

// Slots for fastening things to other things
// Doesn't give you the same benefits as 'equiping' them
// ex: Wearing gloves gives you benefits that
///    fastening them to your belt does not
struct FFastenerData : public IContainerBase
{
public:
    bool CanAddItem(const FAvalonItem& Object) const override;
};

// Slots for holding fluids
struct FVesselData : public IContainerBase
{
public:
    //fluids don't have volume, instead we measure how much they've filled the volume they are in.
    float mContentVolume = 0.f;
};

*/
