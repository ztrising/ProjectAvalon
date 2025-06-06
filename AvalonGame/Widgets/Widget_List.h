/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "AvalonWidget.h"
#include <vector>

struct FWidgetListSettings
{
	unsigned int mMaxHorizontalElements = 1;
	unsigned int mMaxVerticalElements = 4;
	unsigned int mVerticalPadding = 1;
	unsigned int mHorizontalPadding = 1;

	bool PopulateVertically = true;
};

/***************************************************************************************
*  Widget_List
* 
*  It holds a list of children and displays them based on settings
*  ex:  Item Container Item List
****************************************************************************************/

class Widget_List : public AvalonWidget
{
public:
	
	template <class T>
	void ConstructList( unsigned int NumElements, 
						const FWidgetListSettings& Settings, 
						const char* WidgetAsset = nullptr)
	{
		EmptyList<T>();

		if (NumElements == 0)
		{
			return;
		}

		mSettings = Settings;

		for (unsigned int Index = 0; Index < NumElements; ++Index)
		{
			HardUnitRef NewWidget = AddChild<T>(WidgetAsset);
			mListElements.push_back(NewWidget);
		}

		mElementDimensions = Get<AvalonWidget>(mListElements[0])->GetDimensions();

		FCoord Dimensions;
		Dimensions.X = mElementDimensions.X * mSettings.mMaxHorizontalElements;
		Dimensions.X += mSettings.mHorizontalPadding;

		Dimensions.Y = mElementDimensions.X * mSettings.mMaxVerticalElements;
		Dimensions.Y += mSettings.mVerticalPadding;

		mBuffer.AllocateBuffer(Dimensions, AVALON_CHAR_TRANSPARENT, 0);

		RepositionElements();
		//RecalculatePosition();  // TODO, was this necessary?
	}

	template<typename Predicate>
	void ForEachItem(Predicate Func)
	{
		unsigned int NumElements = mListElements.size();
		for (unsigned int Index = 0; Index < NumElements; ++Index)
		{
			Func(mListElements[Index]);
		}
	}

	template <class T>
	void EmptyList()
	{
		unsigned int NumElements = mListElements.size();
		for (unsigned int Index = 0; Index < NumElements; ++Index)
		{
			HardUnitRef WidgetRef = mListElements[Index];
			RemoveChild<T>(WidgetRef);
		}
		mListElements.clear();
	}

	int GetElementIndex(const HardUnitRef& WidgetRef);

private:
	void RepositionElements();

	FWidgetListSettings mSettings;
	FCoord mElementDimensions;

	HardRefList mListElements;

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	virtual void Construct(const char* WidgetAsset) override;
	/****************************************************************************************/
};
