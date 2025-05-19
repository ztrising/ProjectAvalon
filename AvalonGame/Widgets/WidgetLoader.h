/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../../AvalonUtilities/DynamicArray.h"

class AvalonWidget;
struct FFrameBuffer;
struct FCoord;
struct FPositionData;
struct FTextSettings;

namespace tinyxml2 { class XMLElement; }

namespace AssetLoaders
{
    /***************************************************************************************
    *  Avalon Widget
    ****************************************************************************************/
    void CopyFrameBufferTo(const FFrameBuffer& Source, FFrameBuffer& Destination);
    void FillBufferWithString(FFrameBuffer& mBuffer, const char* String);
    void InitPositionData(tinyxml2::XMLElement* PositionData, FPositionData& Data);
    void ApplyAttributesToBuffer(FFrameBuffer& Buffer, unsigned short Attributes);
    void WriteTextToBuffer(FFrameBuffer& Buffer, const char* String, const FTextSettings& Settings);
    void LinearWriteToBuffer(FFrameBuffer& Buffer, const char* String, FCoord Pos, unsigned short Attributes, bool ApplyAttributes);
    void LoadBufferForAnimation(const char* AssetName, FFrameBuffer& Buffer);
    void PopulateBuffer(FFrameBuffer& OutBuffer, tinyxml2::XMLElement* BufferData, const FCoord& ParentExtent, const FCoord& ParentPos);
    void LoadVisualElement(const char* WidgetAsset, AvalonWidget* Widget);
    void LoadWidgetComposition(const char* WidgetAsset, AvalonWidget* Widget);
    /****************************************************************************************/
}
