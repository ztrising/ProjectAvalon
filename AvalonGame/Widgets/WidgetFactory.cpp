/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/
#include "WidgetFactory.h"
#include "AvalonWidget.h"

#include <string>

/***************************************************************************************
*  WidgetFactory
****************************************************************************************/
#include "Widget_LocationPanel.h"
#include "Widget_DialoguePanel.h"
#include "Widget_ChoiceMenu.h"
#include "Widget_GameTime.h"
#include "Widget_Inventory.h"
#include "Widget_PlayerPanel.h"
#include "Widget_Meditate.h"
#include "Widget_Journal.h"
#include "Widget_PlayerEquipment.h"
#include "Widget_PlayerStatus.h"
#include "Widget_Travel.h"

void WidgetFactory::AddChildWithFactory( AvalonWidget* ParentWidget
                                        , const char* ChildClassName
                                        , const char* ChildAssetFileName)
{
	std::string ClassString = ChildClassName;

	if (ClassString == "Widget_LocationPanel")
	{
		ParentWidget->AddChild<Widget_LocationPanel>(ChildAssetFileName);
	}
	else if (ClassString == "Widget_DialoguePanel")
	{
		ParentWidget->AddChild<Widget_DialoguePanel>(ChildAssetFileName);
	}
	else if (ClassString == "Widget_ChoiceMenu")
	{
		ParentWidget->AddChild<Widget_ChoiceMenu>(ChildAssetFileName);
	}
	else if (ClassString == "AvalonWidget")
	{
		ParentWidget->AddChild<AvalonWidget>(ChildAssetFileName);
	}
	else if (ClassString == "Widget_GameTime")
	{
		ParentWidget->AddChild<Widget_GameTime>(ChildAssetFileName);
	}
	else if (ClassString == "Widget_Inventory")
	{
		ParentWidget->AddChild<Widget_Inventory>(ChildAssetFileName);
	}
	else if (ClassString == "Widget_PlayerPanel")
	{
		ParentWidget->AddChild<Widget_PlayerPanel>(ChildAssetFileName);
	}
	else if (ClassString == "Widget_Meditate")
	{
		ParentWidget->AddChild<Widget_Meditate>(ChildAssetFileName);
	}
	else if (ClassString == "Widget_Journal")
	{
		ParentWidget->AddChild<Widget_Journal>(ChildAssetFileName);
	}
	else if (ClassString == "Widget_PlayerEquipment")
	{
		ParentWidget->AddChild<Widget_PlayerEquipment>(ChildAssetFileName);
	}
	else if (ClassString == "Widget_PlayerStatus")
	{
		ParentWidget->AddChild<Widget_PlayerStatus>(ChildAssetFileName);
	}
	else if (ClassString == "Widget_Travel")
	{
		ParentWidget->AddChild<Widget_Travel>(ChildAssetFileName);
	}
}
/****************************************************************************************/
