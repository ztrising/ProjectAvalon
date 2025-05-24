/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_PlayerEquipment.h"
#include "Widget_Equipment.h"

#include "../Gameplay/Actor/PlayerActor.h"
#include "../Gameplay/Components/ContainerComponent.h"
#include "../Gameplay/AvalonGameState.h"

void Widget_PlayerEquipment::ShowEquipmentPanel()
{
	Get<AvalonWidget>(mEquipmentPanelRef)->Show();
}

void Widget_PlayerEquipment::HideEquipmentPanel()
{
	Get<AvalonWidget>(mEquipmentPanelRef)->Hide();
}

/***************************************************************************************
*  Avalon Widget Interface
****************************************************************************************/
void Widget_PlayerEquipment::Construct(const char* WidgetAsset)
{
	Widget_ToggleButton::Construct("W_EquipmentHUDButton.xml");

	mEquipmentPanelRef = AddChild<Widget_Equipment>();
	Get<AvalonWidget>(mEquipmentPanelRef)->Hide();
}

void Widget_PlayerEquipment::OnGameLoaded()
{
	AvalonWidget::OnGameLoaded();

	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	if (Player != nullptr)
	{
		Equipment* EquipComp = Player->GetComponent<Equipment>();
		if (EquipComp != nullptr)
		{
			Widget_Equipment* EquipWidget = Get<Widget_Equipment>(mEquipmentPanelRef);
			if (EquipWidget != nullptr)
			{
				EquipWidget->SetEquipmentRef(EquipComp);
			}
		}
	}
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




