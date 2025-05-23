/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_Meditate.h"

#include "../Gameplay/Actor/PlayerActor.h"
#include "../Gameplay/Components/Meditator.h"
#include "../Gameplay/AvalonGameState.h"

/***************************************************************************************
*  AvalonWidget
****************************************************************************************/
void Widget_Meditate::Construct(const char* WidgetAsset)
{
	Widget_Button::Construct("W_Meditate_Button.xml");
}
/****************************************************************************************/

/***************************************************************************************
*  Widget_Button
****************************************************************************************/
void Widget_Meditate::OnButtonPressed()
{
	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	if (Player != nullptr)
	{
		Player->GetComponent<Meditator>()->StartMeditating(1);
	}
}
/****************************************************************************************/
