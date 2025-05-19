/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include <vector>
#include <string>

namespace tinyxml2 { class XMLElement; class XMLDocument; }
struct IAvalonItemProperty;

struct FAvalonItemID
{
    std::string mFilename;
    std::string mID;
};

struct FAvalonItem
{
public:
    FAvalonItem();
    ~FAvalonItem();

    void PopulateActionsForItem();

    static void ConstrucItemFromID(const FAvalonItemID& ItemID, FAvalonItem& OutItem);

    /***************************************************************************************
    *  Basic Item Properties
    ****************************************************************************************/
public:
    FAvalonItemID mID;

    std::string mDisplayName;
    std::string mDescription;
    /****************************************************************************************/

    /***************************************************************************************
    *  Dynamic Properties!
    ****************************************************************************************/
public:
    template <class T>
    T* GetItemProperty() const
    {
        for (IAvalonItemProperty* Property : mItemProperties) 
        {
            T* RetValue = dynamic_cast<T*>(Property);
            if (RetValue)
            {
                return RetValue;
            }
        }

        return nullptr;
    }

private:
    template <class T>
    void AddItemProperty(tinyxml2::XMLElement* Element)
    {
        T* NewThing = new T;
        IAvalonItemProperty* Property = static_cast<IAvalonItemProperty*>(NewThing);
        Property->mOwner = this;
        Property->LoadPropertyData(Element);

        mItemProperties.push_back(Property);
    }

    std::vector<IAvalonItemProperty*> mItemProperties;
    /****************************************************************************************/
};
