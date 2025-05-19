/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_ToggleButton.h"
#include "Animation/WidgetAnimation.h"

#include "../Gameplay/Actor/ActorTypes.h"

class Widget_LocationOption : public Widget_ToggleButton
{
public:
	Widget_LocationOption();

	void SetActorHandle(FUnitHandle Handle);
	void SetLocationText(const char* Text);

private:
	FUnitHandle mSymbolPrompt; // > or [1]
	FUnitHandle mText;

	FMoveAnimSettings mAnimation;
	FTextSettings mTextSettings;

	FUnitHandle mActorHandle;

	/***************************************************************************************
	*  Avalon Widget Interface
	****************************************************************************************/
public:
	void Construct(const char* WidgetAsset) override;
	void OnFocusGained() override;
	void OnFocusLost() override;
	/****************************************************************************************/

	/***************************************************************************************
	*  Widget_ToggleButton
	****************************************************************************************/
public:
	void SetActive(bool Active) override;
	/****************************************************************************************/
};
