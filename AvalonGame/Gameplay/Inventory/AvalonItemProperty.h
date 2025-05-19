/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include <string>

struct FAvalonItem;

namespace tinyxml2 { class XMLElement; class XMLDocument; }

struct IAvalonItemProperty
{
public:
    virtual void LoadPropertyData(tinyxml2::XMLElement* Element) {};
    FAvalonItem* GetOwner() { return mOwner; }
private:
    FAvalonItem* mOwner;
};

struct FPhysicalData : public IAvalonItemProperty
{
public:
    void LoadPropertyData(tinyxml2::XMLElement* Element) override;

    float GetMass() const;
    float GetVolume() const;

private:
    float mMass = 0;
    float mVolume = 0;
    bool mIsFluid = false;
};

struct FEquipableData : public IAvalonItemProperty
{
public:
    void LoadPropertyData(tinyxml2::XMLElement* Element) override;

    std::string mSlot;
    std::string mLayer;
};

struct FFastenableData : public IAvalonItemProperty
{
public:
    void LoadPropertyData(tinyxml2::XMLElement* Element) override;
};

struct FConsumableData : public IAvalonItemProperty
{
public:
    void LoadPropertyData(tinyxml2::XMLElement* Element) override;
};

struct FStackableData : public IAvalonItemProperty
{
public:
    void LoadPropertyData(tinyxml2::XMLElement* Element) override;
    static bool TryAddStack(FAvalonItem& StackBase, const FAvalonItem& NewItem);

    int mAmount = 1;
    
};

struct FTradeable : public IAvalonItemProperty
{
public:
    void LoadPropertyData(tinyxml2::XMLElement* Element) override;
    int mBaseCost;
};