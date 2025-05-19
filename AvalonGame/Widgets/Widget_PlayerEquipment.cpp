/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_PlayerEquipment.h"
#include "Widget_Equipment.h"

#include "../Gameplay/Actor/PlayerActor.h"
#include "../Gameplay/Components/ContainerComponent.h"

void Widget_PlayerEquipment::ShowEquipmentPanel()
{
	mEquipmentPanel.Get<AvalonWidget>()->Show();
}

void Widget_PlayerEquipment::HideEquipmentPanel()
{
	mEquipmentPanel.Get<AvalonWidget>()->Hide();
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_PlayerEquipment::Construct(const char* WidgetAsset)
{
	Widget_ToggleButton::Construct("W_EquipmentHUDButton.xml");

	mEquipmentPanel = AddChild<Widget_Equipment>();
	mEquipmentPanel.Get<AvalonWidget>()->Hide();
}

void Widget_PlayerEquipment::OnGameLoaded()
{
	AvalonWidget::OnGameLoaded();

	Equipment* EquipCont = PlayerActor::mPlayer->GetComponent<Equipment>();
	Widget_Equipment* EquipWidget = mEquipmentPanel.Get<Widget_Equipment>();
	EquipWidget->SetEquipmentRef(EquipCont);
}
/****************************************************************************************/

/***************************************************************************************
*  Widget_ToggleButton
****************************************************************************************/
void Widget_PlayerEquipment::SetActive(bool Active)
{
	Widget_ToggleButton::SetActive(Active);

	if (mIsActive)
	{
		ShowEquipmentPanel();
	}
	else
	{
		HideEquipmentPanel();
	}
}
/****************************************************************************************/




