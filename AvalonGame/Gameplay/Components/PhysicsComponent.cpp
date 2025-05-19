/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "PhysicsComponent.h"

/*void PhysicsComponent::Initialize(tinyxml2::XMLElement* Element)
{
    Element->QueryFloatAttribute("Mass", &mMass);
    Element->QueryFloatAttribute("Volume", &mVolume);
    Element->QueryBoolAttribute("IsFluid", &mIsFluid);
}*/

/*
float PhysicsComponent::GetMass() const
{
    float RetMass = mMass;

    if (mContainerData != nullptr)
    {
        RetMass += mContainerData->GetContainerMass();
    }

    if (mStackableData != nullptr)
    {
        RetMass *= mStackableData->mAmount;
    }

    return RetMass;
}

float PhysicsComponent::GetVolume() const
{
    float RetVolume = mVolume;

    if (mStackableData != nullptr)
    {
        RetVolume *= mStackableData->mAmount;
    }

    return RetVolume;
}*/
