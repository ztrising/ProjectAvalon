/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_ToggleButton.h"

struct FBufferAnimSettings;

class Widget_ItemPouch : public Widget_ToggleButton
{
public:
	Widget_ItemPouch();
	~Widget_ItemPouch();

private:
	FUnitHandle mPouchImage;

	FBufferAnimSettings* mPouchOpenAnimation;
	FBufferAnimSettings* mPouchCloseAnimation;

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	/****************************************************************************************/

	/***************************************************************************************
	*  Widget_ToggleButton
	****************************************************************************************/
public:
	virtual void SetActive(bool Active);
	/****************************************************************************************/
};
