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

	void SetLocationActorRef(HardUnitRef ActorRef);
	void SetLocationText(const char* Text);

private:
	HardUnitRef mSymbolPromptRef; // > or [1]
	HardUnitRef mTextWidgetRef;

	FMoveAnimSettings mAnimation;
	FTextSettings mTextSettings;

	SoftUnitRef mLocationActorRef;

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
