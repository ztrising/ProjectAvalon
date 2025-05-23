/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonUnit.h"
#include "../Gameplay/AvalonGameState.h"

void IAvalonUnit::OnCreated(HardUnitRef UnitRef)
{ 
    mSelfRef = UnitRef; 
}

bool IAvalonUnit::operator==(const IAvalonUnit& rhs) const
{
    return (mSelfRef.lock() == rhs.mSelfRef.lock());
}

bool IAvalonUnit::operator!=(const IAvalonUnit& rhs) const
{
    return (mSelfRef.lock() != rhs.mSelfRef.lock());
}