/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "AvalonTypes.h"

void AvalonUtility::StringCpy(const char* Source, char* Destination, int Size)
{
	for (int i = 0; i < Size; i++)
		Destination[i] = Source[i];
}

