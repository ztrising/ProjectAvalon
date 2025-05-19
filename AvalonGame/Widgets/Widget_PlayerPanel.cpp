/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "Widget_PlayerPanel.h"

Widget_PlayerPanel::Widget_PlayerPanel()
{
	mStartVisible = false;
}

void Widget_PlayerPanel::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct("W_PlayerPanel_Frame.xml");
}

void Widget_PlayerPanel::OnGameLoaded()
{
	AvalonWidget::OnGameLoaded();
	Show();
}
