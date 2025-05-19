/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "StatTypes.h"

#include <algorithm>

/***************************************************************************************
*   Attribute Mod
****************************************************************************************/
void FAttributeMod::ApplyModToValue(float& OutValue) const
{
	switch (mOperation)
	{
	case EModOperation::PreAdd:
	case EModOperation::Add:		OutValue += mValue;	break;
	case EModOperation::Multiply:	OutValue *= mValue;	break;
	case EModOperation::Override:	OutValue = mValue;	break;
	}
}

bool FAttributeMod::operator<(const FAttributeMod& rhs) const
{
	return mOperation < rhs.mOperation;
}

void FAttributeMod::Load(FSaveContext& Context)
{
	std::string OperationString;
	Context.Load("Operation", OperationString);

	if (OperationString == "PreAdd")
	{
		mOperation = EModOperation::PreAdd;
	}
	else if (OperationString == "Add")
	{
		mOperation = EModOperation::Add;
	}
	else if (OperationString == "Multiply")
	{
		mOperation = EModOperation::Multiply;
	}
	else if (OperationString == "Override")
	{
		mOperation = EModOperation::Override;
	}

	Context.Load("Value", mValue);
}

void FAttributeMod::Save(FSaveContext& Context)
{
	std::string OperationString;
	switch (mOperation)
	{
	case EModOperation::PreAdd:		OperationString = "PreAdd"; break;
	case EModOperation::Add:		OperationString = "Add"; break;
	case EModOperation::Multiply:	OperationString = "Multiply"; break;
	case EModOperation::Override:	OperationString = "Override"; break;
	}

	Context.Save("Operation", OperationString);
	Context.Save("Value", mValue);
}

/***************************************************************************************
*   Avalon Attribute
****************************************************************************************/
float FAvalonAttribute::GetValue()
{
	return mCachedEvaluatedValue;
}

void FAvalonAttribute::AddModifier(FAttributeMod Mod)
{
	auto it = std::upper_bound(mMods.cbegin(), mMods.cend(), Mod);
	mMods.insert(it, Mod);

	EvaluateAttribute();
}

void FAvalonAttribute::RemoveModifier(FAttributeMod Mod)
{
	std::vector<FAttributeMod>::iterator position = std::find(mMods.begin(), mMods.end(), Mod);
	if (position != mMods.end())
	{
		mMods.erase(position);
	}

	EvaluateAttribute();
}

void FAvalonAttribute::EvaluateAttribute()
{
	float OldValue = mCachedEvaluatedValue;

	mCachedEvaluatedValue = mBaseValue;

	for (FAttributeMod& Mod : mMods)
	{
		Mod.ApplyModToValue(mCachedEvaluatedValue);
	}

	if (OldValue != mCachedEvaluatedValue)
	{
		// Tell Attribute Change Listeners
	}
}

void FAvalonAttribute::Load(FSaveContext& Context)
{
	Context.Load("BaseValue", mBaseValue);

	/*auto LoadMod_Lamdba = [&](FSaveContext& InContext)
	{
		FAttributeMod NewMod;
		ISaveable::Load(&NewMod, InContext);
		AddModifier(NewMod);
		
	};

	Context.LoadFromWrapper("Mods", LoadMod_Lamdba);*/

	EvaluateAttribute();
}

void FAvalonAttribute::Save(FSaveContext& Context)
{
	Context.Save("BaseValue", mBaseValue);
	
	// TODO: make saveable utility for non-pointer arrays
	//  OR make these pointers (probably the latter)
	//Context.SaveUnderWrapper("Mods", mMods);
}

/***************************************************************************************
*  FAvalonStat
****************************************************************************************/
void FAvalonStat::Load(FSaveContext& Context)
{
	// Expected Layout:
	/*
		<Health CurrentValue="100">
			<MinValue BaseValue="0">
				<Mod ... />
				<Mod ... />
			</MinValue>
			<MaxValue BaseValue="100">
				<Mod ... />
				<Mod ... />
			</MaxValue>
			<Regen BaseValue="1">
				<Mod ... />
				<Mod ... />
			</Regen>
		</Health>
	*/

	Context.Load("CurrentValue", mCurrentValue);

	FSaveContext MinContext;
	Context.GetChild(MinContext, "MinValue");
	ISaveable::Load(&mMinValue, MinContext);

	FSaveContext MaxContext;
	Context.GetChild(MaxContext, "MaxValue");
	ISaveable::Load(&mMaxValue, MaxContext);

	FSaveContext RegenContext;
	Context.GetChild(RegenContext, "Regen");
	ISaveable::Load(&mRegen, RegenContext);
}

void FAvalonStat::Save(FSaveContext& Context)
{
	Context.Save("CurrentValue", mCurrentValue);
	ISaveable::Save(&mMinValue, Context);
	ISaveable::Save(&mMaxValue, Context);
	ISaveable::Save(&mRegen, Context);
}


void FAvalonStat::SetCurrentValue(float NewValue)
{
	float OldValue = mCurrentValue;

	float Min = mMinValue.GetValue();
	float Max = mMaxValue.GetValue();

	if (NewValue < Min)
	{
		NewValue = Min;
	}
	else if (NewValue > Max)
	{
		NewValue = Max;
	}
	else
	{
		mCurrentValue = NewValue;
	}

	if (mCurrentValue != OldValue)
	{
		mChangedThisFrame = true;
	}
}

