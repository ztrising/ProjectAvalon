/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

/*

void OpenLocationAsset(const char* LocFile,
					   tinyxml2::XMLDocument& InDocument,
					   tinyxml2::XMLElement*& OutLocElement)
{
	AssetLoaders::OpenGameDataFile(LocFile, InDocument);

	tinyxml2::XMLElement* AvalonData = InDocument.FirstChildElement("AvalonData");
	OutLocElement = AvalonData->FirstChildElement("AvalonLocation");
}

void AssetLoaders::LoadLocationActions(const char* LocFile, const char* PointOfInterest, DynamicArray<FAvalonAction*>& OutActions)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* LocationElement = nullptr;
	OpenLocationAsset(LocFile, doc, LocationElement);

	tinyxml2::XMLElement* PoIs = LocationElement->FirstChildElement("PointsOfInterest");
	tinyxml2::XMLElement* Element = PoIs->FirstChildElement(PointOfInterest);
	AssetLoaders::PopulateActions(LocFile, Element, OutActions);
}

void AssetLoaders::LoadTravelActions(const char* LocFile, DynamicArray<FAvalonAction*>& OutActions)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* LocationElement = nullptr;
	OpenLocationAsset(LocFile, doc, LocationElement);

	AssetLoaders::PopulateActions(LocFile, LocationElement, OutActions);
}

void AssetLoaders::PopulateLocationFromSave(const char* SaveFile, FLocationInfo& OutLocation)
{
	tinyxml2::XMLDocument doc;
	AssetLoaders::OpenAvalonFile(SaveFile, doc);

	tinyxml2::XMLElement* AvalonData = doc.FirstChildElement("AvalonData");
	tinyxml2::XMLElement* JournalRoot = AvalonData->FirstChildElement("Journal");
	tinyxml2::XMLElement* LastEntry = JournalRoot->LastChildElement("JournalEntry");
	tinyxml2::XMLElement* LocationElement = LastEntry->FirstChildElement("AvalonLocation");

	const char* FileName = "failed";
	LocationElement->QueryStringAttribute("LocFile", &FileName);
	AssetLoaders::PopulateLocationFromFile(FileName, OutLocation);
}

void AssetLoaders::PopulateLocationFromFile(const char* LocFile, FLocationInfo& OutLocation)
{
	unsigned int NumEntries = OutLocation.mPointsOfInterest.Num();
	for (unsigned int Index = 0; Index < NumEntries; ++Index)
	{
		FPointOfInterest* Entry = OutLocation.mPointsOfInterest[Index];
		delete Entry;
	}
	OutLocation.mPointsOfInterest.Clear();

	strncpy_s(OutLocation.mFileName, AVALON_MAX_TEXT_SML, LocFile, _TRUNCATE);

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* LocationElement = nullptr;
	OpenLocationAsset(LocFile, doc, LocationElement);

	// Basic Info
	if (tinyxml2::XMLElement* DisplayInfo = LocationElement->FirstChildElement("LocInfo"))
	{
		const char* NameString = "failed";
		DisplayInfo->QueryStringAttribute("RegionName", &NameString);
		strncpy_s(OutLocation.mRegionName, AVALON_MAX_TEXT_SML, NameString, AVALON_MAX_TEXT_SML);

		DisplayInfo->QueryStringAttribute("AreaName", &NameString);
		strncpy_s(OutLocation.mAreaName, AVALON_MAX_TEXT_SML, NameString, AVALON_MAX_TEXT_SML);

		DisplayInfo->QueryStringAttribute("LocName", &NameString);
		strncpy_s(OutLocation.mLocName, AVALON_MAX_TEXT_SML, NameString, AVALON_MAX_TEXT_SML);

		DisplayInfo->QueryStringAttribute("Desc", &NameString);
		strncpy_s(OutLocation.mLocDesc, AVALON_MAX_TEXT_LRG, NameString, AVALON_MAX_TEXT_LRG);
	}

	// Points of Interest
	if (tinyxml2::XMLElement* PoIs = LocationElement->FirstChildElement("PointsOfInterest"))
	{
		tinyxml2::XMLElement* PointOfInterest = PoIs->FirstChildElement();
		while (PointOfInterest != nullptr)
		{
			FPointOfInterest* NewPoI = new FPointOfInterest();
			strncpy_s(NewPoI->mID, AVALON_MAX_TEXT_SML, PointOfInterest->Value(), AVALON_MAX_TEXT_SML);

			tinyxml2::XMLElement* DescElement = PointOfInterest->FirstChildElement("Desc");

			const char* Text = "failure";
			DescElement->QueryStringAttribute("Text", &Text);
			strncpy_s(NewPoI->mDesc, AVALON_MAX_TEXT_LRG, Text, AVALON_MAX_TEXT_LRG);
			OutLocation.mPointsOfInterest.Add(NewPoI);

			PointOfInterest = PointOfInterest->NextSiblingElement();
		}
	}
}

FTravelRequest* AssetLoaders::MakeNewTravelRequest(const FTravelRequest& Request)
{
	FTravelRequest* NewRequest = new FTravelRequest();

	strncpy_s(NewRequest->mLocFile, AVALON_MAX_TEXT_SML, Request.mLocFile, AVALON_MAX_TEXT_SML);
	NewRequest->mTravelTime = Request.mTravelTime;

	return NewRequest;
}

*/