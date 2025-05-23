/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_List.h"
#include "../../AvalonViewport/FrameBuffer/FrameBuffer.h"

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_List::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct(nullptr);
}
/****************************************************************************************/

int Widget_List::GetElementIndex(const HardUnitRef& WidgetRef)
{
	for (unsigned int Index = 0; Index < mListElements.size(); ++Index)
	{
		if (WidgetRef == mListElements[Index])
		{
			return Index;
		}
	}

	return -1;
}

void Widget_List::RepositionElements()
{
	unsigned int XSlot = 0;
	unsigned int YSlot = 0;

	unsigned int NumElements = mListElements.size();
	for (unsigned int Index = 0; Index < NumElements; ++Index)
	{
		HardUnitRef WidgetRef = mListElements[Index];
		AvalonWidget* Widget = Get<AvalonWidget>(WidgetRef);
		
		FCoord NewPosition;
		NewPosition.X = (XSlot * mElementDimensions.X) + (XSlot * mSettings.mHorizontalPadding);
		NewPosition.Y = (YSlot * mElementDimensions.Y) + (YSlot * mSettings.mVerticalPadding);
		Widget->SetPosition(NewPosition);

		if (mSettings.PopulateVertically)
		{
			YSlot++;
			if (YSlot >= mSettings.mMaxVerticalElements)
			{
				YSlot = 0;
				XSlot++;
			}
		}
		else
		{
			XSlot++;
			if (XSlot >= mSettings.mMaxHorizontalElements)
			{
				XSlot = 0;
				YSlot++;
			}
		}
	}
}
