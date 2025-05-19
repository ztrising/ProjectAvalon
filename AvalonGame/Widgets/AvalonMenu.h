/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

class IAvalonMenu
{
public:
	IAvalonMenu() {}

	/***************************************************************************************
	*  IAvalonMenu Interface
	****************************************************************************************/
public:
	virtual void ShowMenu() = 0;
	virtual void HideMenu() = 0;

	virtual void NavigateLeft() {};
	virtual void NavigateRight() {};

	virtual void NavigateUp() {};
	virtual void NavigateDown() {};
	/****************************************************************************************/
};

