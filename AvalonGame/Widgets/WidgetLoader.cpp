/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "WidgetLoader.h"
#include "AvalonWidget.h"
#include "AvalonHUD.h"
#include "WidgetFactory.h"

#include "../../AvalonViewport/FrameBuffer/FrameBuffer.h"
#include "../../AvalonViewport/AvalonViewportManager.h"

#include "../AssetLoaders/AssetLoader.h"

#include "../../ExternalTools/TinyXML/tinyxml2.h"
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

unsigned short GetColorOptionFromString(const char* String, bool IsBG)
{
	if (strcmp(String, "Blue") == 0)
	{
		return IsBG ? AVALON_BG_BLUE : AVALON_FG_BLUE;
	}
	else if (strcmp(String, "White") == 0)
	{
		return IsBG ? AVALON_BG_WHITE : AVALON_FG_WHITE;
	}
	else if (strcmp(String, "Red") == 0)
	{
		return IsBG ? AVALON_BG_RED : AVALON_FG_RED;
	}
	else if (strcmp(String, "Green") == 0)
	{
		return IsBG ? AVALON_BG_GREEN : AVALON_FG_GREEN;
	}
	else if (strcmp(String, "Cyan") == 0)
	{
		return IsBG ? AVALON_BG_CYAN : AVALON_FG_CYAN;
	}
	else if (strcmp(String, "Magenta") == 0)
	{
		return IsBG ? AVALON_BG_MAGENTA : AVALON_FG_MAGENTA;
	}
	else if (strcmp(String, "Yellow") == 0)
	{
		return IsBG ? AVALON_BG_YELLOW : AVALON_FG_YELLOW;
	}

	// Black
	return 0;
}

void PopulateAnimations(AvalonWidget* Element, tinyxml2::XMLElement* AnimationData)
{
	// Let's assume its a fade to for now
	/* {
		BufferAnimation_FadeTo* NewAnim = new BufferAnimation_FadeTo();

		// For FadeTos we expect a single Buffer End Frame
		tinyxml2::XMLElement* BufferData = AnimationData->FirstChildElement("BufferLayer");
		AssetLoader::PopulateBuffer(NewAnim->mEndValue, BufferData, Element->mBuffer->GetSize(), Element->mBuffer->GetPos());

		// And a length
		if (tinyxml2::XMLElement* AnimInfo = AnimationData->FirstChildElement("AnimType"))
		{
			float AnimLength = 0.f;
			if (AnimInfo->QueryFloatAttribute("Length", &AnimLength) == tinyxml2::XML_SUCCESS)
			{
				NewAnim->mLength = AnimLength;
			}
		}

		// All animations need to a destination buffer
		NewAnim->mDestinationBuffer = Element->mBuffer;

		Element->mLoadedAnimations.Add(NewAnim);
	}*/
}

void AssetLoaders::CopyFrameBufferTo(const FFrameBuffer& Source, FFrameBuffer& Destination)
{
	if (Source.GetLength() != Destination.GetLength())
	{
		Destination.AllocateBuffer(Source.GetSize());
	}

	unsigned int BufferSize = Source.GetLength();
	for (unsigned int Index = 0; Index < BufferSize; ++Index)
	{
		Destination[Index].mChar = Source[Index].mChar;
		Destination[Index].mAttributes = Source[Index].mAttributes;
	}
}

void AssetLoaders::FillBufferWithString(FFrameBuffer& mBuffer, const char* String)
{
	FCoord BufferBounds = mBuffer.GetSize();
	unsigned int BufferSize = BufferBounds.X * BufferBounds.Y;
	unsigned int StringSize = strlen(String);

	unsigned int StringIndex = 0;
	for (unsigned int Index = 0; Index < BufferSize; ++Index)
	{
		FCellData& CellData = mBuffer[Index];

		if (StringIndex < StringSize)
		{
			CellData.mChar = static_cast<unsigned short>(String[Index]);
		}

		++StringIndex;
	}
}

void AssetLoaders::InitPositionData(tinyxml2::XMLElement* PositionData, FPositionData& Data)
{
	if (PositionData)
	{
		// Padding/offset, absolute right now, but should convert to screen percentage??
		int offset;
		if (PositionData->QueryIntAttribute("x", &offset) == tinyxml2::XML_SUCCESS)
		{
			Data.Offset.X = static_cast<short>(offset);
		}

		if (PositionData->QueryIntAttribute("y", &offset) == tinyxml2::XML_SUCCESS)
		{
			Data.Offset.Y = static_cast<short>(offset);
		}

		// Anchoring rules
		const char* AnchorString = "failed";
		PositionData->QueryStringAttribute("anchorH", &AnchorString);
		if (strcmp(AnchorString, "Center") == 0)
		{
			Data.HRule = EHorizontalAnchor::ECenter;
		}
		else if (strcmp(AnchorString, "Right") == 0)
		{
			Data.HRule = EHorizontalAnchor::ERight;
		}

		PositionData->QueryStringAttribute("anchorV", &AnchorString);
		if (strcmp(AnchorString, "Center") == 0)
		{
			Data.VRule = EVerticalAnchor::ECenter;
		}
		else if (strcmp(AnchorString, "Bottom") == 0)
		{
			Data.VRule = EVerticalAnchor::EBottom;
		}

		// Pivot settings
		PositionData->QueryFloatAttribute("pivotX", &Data.PivotX);
		PositionData->QueryFloatAttribute("pivotY", &Data.PivotY);

		// Viewport relative?
		PositionData->QueryBoolAttribute("ViewportRelative", &Data.ViewportRelative);
	}
}

void AssetLoaders::ApplyAttributesToBuffer(FFrameBuffer& Buffer, unsigned short Attributes)
{
	unsigned int BufferLength = Buffer.GetLength();
	for (unsigned int Index = 0; Index < BufferLength; ++Index)
	{
		Buffer[Index].mAttributes = Attributes;
	}
}

void AssetLoaders::WriteTextToBuffer(FFrameBuffer& Buffer, const char* String, const FTextSettings& Settings)
{
	std::string Text = String;

	//strip it of end lines and tabs
	Text.erase(remove(Text.begin(), Text.end(), '\t'), Text.end());
	Text.erase(remove(Text.begin(), Text.end(), '\n'), Text.end());

	unsigned int BufferWidth	= Buffer.GetSize().X;
	unsigned int BufferLength	= Buffer.GetLength();

	unsigned int TextBoxHeight	= Settings.mUseBufferExtentHeight? Buffer.GetSize().Y : Settings.mExtent.Y;
	unsigned int TextBoxWidth	= Settings.mUseBufferExtentWidth ? BufferWidth : Settings.mExtent.X;
	unsigned int TextBoxSize	= TextBoxWidth * TextBoxHeight;
	unsigned int TextSize		= Text.size();

	if (TextSize == 0 || TextBoxSize == 0)
	{
		return;
	}

	char FillChar = Settings.mFillChar;
	std::string FillLineString = "";
	if (Settings.mFillExtent)
	{
		int Index = 0;
		while (Index < TextBoxWidth)
		{
			FillLineString = FillLineString + FillChar;
			Index++;
		}
	}

	// for ref:
	//unsigned int ConvertedIndex = (Pos.Y * Buffer.GetSize().X) + Pos.X;

	std::vector<std::string> WordsInText;
	{
		std::stringstream stream(Text);
		std::string Word;
		while (std::getline(stream, Word, ' '))
		{
			WordsInText.push_back(Word);
		}
	}

	unsigned int WordIndex = 0;
	unsigned int CharacterIndex = 0;
	unsigned int NumWords = WordsInText.size();
	std::string CurrentWord = WordsInText[WordIndex];
	unsigned int WordSize = CurrentWord.size();


	unsigned int Line = 0;
	while (Line < TextBoxHeight)
	{
		// Figure out how many words will fit in the line
		unsigned int WordsInLine = 0;
		unsigned int CharacterCount = 0;
		std::string LineString;

		for (unsigned int Index = WordIndex; Index < NumWords; ++Index)
		{
			unsigned int WordSize = WordsInText[Index].size();
			CharacterCount += WordSize;

			if (CharacterCount < TextBoxWidth)
			{
				WordsInLine++;
				LineString = LineString + WordsInText[Index];
			}
			else
			{
				CharacterCount -= WordSize;
				break;
			}

			if (Index < (NumWords - 1) && CharacterCount < (TextBoxWidth - 1))
			{
				CharacterCount++; // Space after word
				if (CharacterCount >= TextBoxWidth)
				{
					CharacterCount--;
					break;
				}

				LineString = LineString + ' ';
			}
		}

		// Fill the line with the FILL character
		if (Settings.mFillExtent)
		{
			// Write the fill line
			FCoord LinePosition;
			LinePosition.X = Settings.mOffset.X;
			LinePosition.Y = Line + Settings.mOffset.Y;

			LinearWriteToBuffer(Buffer, FillLineString.c_str(), LinePosition, Settings.mAttributes, Settings.mApplyAttributes);
		}


		if (WordsInLine > 0)
		{
			// Figure out where to start the line based on alignment settings
			unsigned int LineStartPosition = 0;
			if (Settings.mJustified == ETextJustification::Center)
			{
				LineStartPosition = ((TextBoxWidth - 1) - CharacterCount) / 2;
			}
			else if (Settings.mJustified == ETextJustification::Right)
			{
				LineStartPosition = (TextBoxWidth - 1) - CharacterCount;
			}

			// Write the line
			FCoord LinePosition;
			LinePosition.X = LineStartPosition + Settings.mOffset.X;
			LinePosition.Y = Line + Settings.mOffset.Y;

			LinearWriteToBuffer(Buffer, LineString.c_str(), LinePosition, Settings.mAttributes, Settings.mApplyAttributes);
		}

		WordIndex += WordsInLine;
		Line++;
	}
}

void AssetLoaders::LinearWriteToBuffer(FFrameBuffer& Buffer, const char* String, FCoord Pos, unsigned short Attributes, bool ApplyAttributes)
{
	unsigned int BufferLength = Buffer.GetLength();
	unsigned int TextLength = strlen(String);

	unsigned int ConvertedIndex = (Pos.Y * Buffer.GetSize().X) + Pos.X;

	for (unsigned int Index = 0; Index < TextLength; ++Index)
	{
		unsigned int AccessIndex = ConvertedIndex + Index;
		if (AccessIndex >= BufferLength)
		{
			return;
		}

		unsigned short Char = static_cast<unsigned short>(String[Index]);
		Buffer[AccessIndex].mChar = Char;

		if (ApplyAttributes)
		{
			Buffer[AccessIndex].mAttributes = Attributes;
		}
	}
}

void AssetLoaders::LoadBufferForAnimation(const char* AssetName, FFrameBuffer& Buffer)
{
	tinyxml2::XMLDocument doc;
	AssetLoaders::OpenWidgetFile(AssetName, doc);

	tinyxml2::XMLElement* AvalonData = doc.FirstChildElement("AvalonData");

	tinyxml2::XMLElement* DisplayData = AvalonData->FirstChildElement("AvalonVisElement");
	FCoord OuterExtent = AvalonViewportManager::GetViewportManager().GetViewportSize();
	FCoord OuterPos = FCoord();

	FPositionData PositionData = FPositionData();


	if (tinyxml2::XMLElement* OffsetData = DisplayData->FirstChildElement("Position"))
	{
		AssetLoaders::InitPositionData(OffsetData, PositionData);
	}

	// Load the buffer
	if (tinyxml2::XMLElement* BufferData = DisplayData->FirstChildElement("BufferLayer"))
	{
		PopulateBuffer(Buffer, BufferData, OuterExtent, OuterPos);
	}
}

void AssetLoaders::PopulateBuffer(FFrameBuffer& OutBuffer, 
								  tinyxml2::XMLElement* BufferData, 
								  const FCoord& ParentExtent, 
								  const FCoord& ParentPos)
{
	FCoord ViewportExtent = AvalonViewportManager::GetViewportManager().GetViewportSize();

	while (BufferData != nullptr)
	{
		FCoord SubBufferPos = FCoord();
		FCoord SubBufferExtent = FCoord();

		// Set up its attributes
		unsigned short Attributes = 0;
		char FillChar = AVALON_CHAR_TRANSPARENT;
		if (tinyxml2::XMLElement* ColorData = BufferData->FirstChildElement("Color"))
		{
			const char* ColorString = "failed";
			ColorData->QueryStringAttribute("BG", &ColorString);
			unsigned short BGColorOption = GetColorOptionFromString(ColorString, true);
			Attributes = BGColorOption;

			ColorData->QueryStringAttribute("FG", &ColorString);
			unsigned short FGColorOption = GetColorOptionFromString(ColorString, false);
			Attributes = Attributes | FGColorOption;

			bool Intensity = false;
			ColorData->QueryBoolAttribute("FGIntensity", &Intensity);
			if (Intensity)
			{
				Attributes = Attributes | AVALON_FG_INTENSITY;
			}

			const char* charString = "$";
			ColorData->QueryStringAttribute("FillChar", &charString);
			FillChar = charString[0];
		}

		unsigned int RowTextWidth = 0;
		unsigned int RowTextHeight = 0;

		// Figure out how big it should be
		{
			// Fill option, draws a square of the FillChar, size defaults to size of parent
			tinyxml2::XMLElement* DrawData = BufferData->FirstChildElement("Fill");
			if (DrawData)
			{
				// Check for fill options
				float posValue = ViewportExtent.X;
				DrawData->QueryFloatAttribute("FillX", &posValue);
				SubBufferExtent.X = posValue;

				posValue = ViewportExtent.Y;
				DrawData->QueryFloatAttribute("FillY", &posValue);
				SubBufferExtent.Y = posValue;
			}

			// Calculate the size of the draw text
			DrawData = BufferData->FirstChildElement("RowTxt");
			while (DrawData != nullptr)
			{
				RowTextHeight++;

				if (RowTextWidth == 0)
				{
					const char* RowString = DrawData->GetText();
					RowTextWidth = strlen(RowString);
				}

				DrawData = DrawData->NextSiblingElement();
			}

			// Expand the extend for the RowText if needed
			SubBufferExtent.X = (RowTextWidth > SubBufferExtent.X) ? RowTextWidth : SubBufferExtent.X;
			SubBufferExtent.Y = (RowTextHeight > SubBufferExtent.Y) ? RowTextHeight : SubBufferExtent.Y;
		}

		// Now figure out the position, by default relative to the parent of all the layers
		if (tinyxml2::XMLElement* OffsetData = BufferData->FirstChildElement("Position"))
		{
			FPositionData PosData = FPositionData();
			AssetLoaders::InitPositionData(OffsetData, PosData);

			bool IsAbsolute = false;
			OffsetData->QueryBoolAttribute("ViewportRelative", &IsAbsolute);

			if (IsAbsolute)
			{
				SubBufferPos = FFrameBuffer::GetWorldPosition(SubBufferExtent, FCoord(), ViewportExtent, PosData);
			}
			else
			{
				SubBufferPos = FFrameBuffer::GetWorldPosition(SubBufferExtent, ParentPos, ParentExtent, PosData);
			}
		}

		// Allocate a temp buffer, this also 'draws' the Fill option
		FFrameBuffer* SubBuffer = new FFrameBuffer();
		SubBuffer->AllocateBuffer(SubBufferExtent, FillChar, Attributes);
		SubBuffer->SetPos(SubBufferPos);

		// Write the Row Text contents
		FCoord CellPos = FCoord(); // Should change this to always (0,0)???
		tinyxml2::XMLElement* RowData = BufferData->FirstChildElement("RowTxt");
		while (RowData != nullptr)
		{
			const char* RowString = RowData->GetText();
			for (unsigned int Index = 0; Index < RowTextWidth; ++Index)
			{
				unsigned short Char = static_cast<unsigned short>(RowString[Index]);
				SubBuffer->SetCellData(CellPos, Char, Attributes);
				CellPos.X++;
			}

			CellPos.Y++;
			CellPos.X = 0;
			RowData = RowData->NextSiblingElement();
		}

		// Collapse the buffer onto the previous buffer
		FFrameBuffer::CollapseBuffersDown(*SubBuffer, OutBuffer, true);
		delete SubBuffer;

		BufferData = BufferData->NextSiblingElement();
	}
}

void AssetLoaders::LoadVisualElement(const char* FileName, AvalonWidget* Element)
{
	tinyxml2::XMLDocument doc;
	AssetLoaders::OpenWidgetFile(FileName, doc);

	if (doc.Error())
	{
		return;
	}

	tinyxml2::XMLElement* AvalonData = doc.FirstChildElement("AvalonData");
	tinyxml2::XMLElement* DisplayData = AvalonData->FirstChildElement("AvalonVisElement");
	FCoord OuterExtent = FCoord();
	FCoord OuterPos = FCoord();

	if (tinyxml2::XMLElement* OffsetData = DisplayData->FirstChildElement("Position"))
	{
		bool IsAbsolute = false;
		OffsetData->QueryBoolAttribute("ViewportRelative", &IsAbsolute);

		AvalonWidget* Parent = Element->GetParent();
		if (IsAbsolute || Parent == nullptr)
		{
			OuterExtent = AvalonViewportManager::GetViewportManager().GetViewportSize();
		}
		else
		{
			OuterExtent = Parent->mBuffer.GetSize();
			OuterPos = Parent->mBuffer.GetPos();
		}

		AssetLoaders::InitPositionData(OffsetData, Element->mPositionData);
	}

	// Load the buffer
	if (tinyxml2::XMLElement* BufferData = DisplayData->FirstChildElement("BufferLayer"))
	{
		PopulateBuffer(Element->mBuffer, BufferData, OuterExtent, OuterPos);
	}

	// Load any animations
	//if (tinyxml2::XMLElement* AnimationData = DisplayData->FirstChildElement("Animation"))
	//{
	//	PopulateAnimations(Element, AnimationData);
	//}
}

void AssetLoaders::LoadWidgetComposition(const char* FileName, AvalonWidget* Widget)
{
	tinyxml2::XMLDocument doc;
	AssetLoaders::OpenWidgetFile(FileName, doc);

	if (doc.Error())
	{
		return;
	}

	tinyxml2::XMLElement* AvalonData = doc.FirstChildElement("AvalonData");
	tinyxml2::XMLElement* DisplayData = AvalonData->FirstChildElement("WidgetComposition");

	if (DisplayData)
	{
		tinyxml2::XMLElement* ChildElement = DisplayData->FirstChildElement();
		while (ChildElement != nullptr)
		{
			const char* ChildAssetFileName = "";
			ChildElement->QueryStringAttribute("WidgetAsset", &ChildAssetFileName);

			const char* ChildClassName = ChildElement->Value();

			WidgetFactory::AddChildWithFactory(Widget, ChildClassName, ChildAssetFileName);

			ChildElement = ChildElement->NextSiblingElement();
		}
	}
}