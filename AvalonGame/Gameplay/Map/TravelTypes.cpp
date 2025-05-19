/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "TravelTypes.h"
#include "../Utility/Saveable.h"

#include <sstream>
#include <algorithm>
#include <vector>

FLocationInfo* FLocationInfo::mWorldMap = nullptr;

/***************************************************************************************
*  TraveTypes.h - Implementation
****************************************************************************************/
/***************************************************************************************
*  FWorldPos
****************************************************************************************/
void FWorldPos::Load(const char* ID, FSaveContext& Context)
{
	// Ex:  "Kaedwen.KaerMorhen.GreatHall"
	std::string StreamString;
	Context.Load(ID, StreamString);

	std::vector<std::string> StringVector;
	{
		std::stringstream stream(StreamString);
		std::string LocString;
		while (std::getline(stream, LocString, '.'))
		{
			StringVector.insert(StringVector.begin(), LocString);
		}
	}

	int Scale = 0;
	for (std::string Str : StringVector)
	{
		mPos[Scale] = Str;
		Scale++;
	}
}

void FWorldPos::Save(const char* ID, FSaveContext& Context)
{
	// TODO: Save
}

const std::string& FWorldPos::operator[](unsigned int index) const
{ 
	return mPos[index]; 
}

std::string& FWorldPos::operator[](unsigned int index)
{
	return mPos[index];
}

FWorldPos& FWorldPos::operator=(const FWorldPos& other)
{
	int Scale = 0;
	while (Scale < EGeoScale::EGS_Count)
	{
		mPos[Scale] = other.mPos[Scale];
		Scale++;
	}

	return *this;
}

bool operator==(const FWorldPos& lhs, const FWorldPos& rhs)
{
	int Scale = 0;
	while (Scale < EGeoScale::EGS_Count)
	{
		if (lhs.mPos[Scale] != rhs.mPos[Scale])
		{
			return false;
		}
		Scale++;
	}

	return true;
}

bool FWorldPos::IsValid() const
{
	int Scale = 0;
	while (Scale < EGeoScale::EGS_Count)
	{
		if (mPos[Scale] == "")
		{
			return false;
		}
		Scale++;
	}

	return true;
}
/****************************************************************************************/

/***************************************************************************************
*  FPathRequest
****************************************************************************************/
EGeoScale GetGeoScaleFromString(std::string GeoScaleString)
{
	EGeoScale RetValue = EGS_Region;

	if (GeoScaleString == "Location")
	{
		RetValue = EGeoScale::EGS_Location;
	}
	else if (GeoScaleString == "Area")
	{
		RetValue = EGeoScale::EGS_Area;
	}

	return RetValue;
}

std::string GetStringFromGeoScale(EGeoScale GeoScale)
{
	std::string RetValue = "INVALID_GEO_SCALE_NAME";
	switch (GeoScale)
	{
	case EGeoScale::EGS_Location:	RetValue = "Location"; break;
	case EGeoScale::EGS_Area:		RetValue = "Area"; break;
	case EGeoScale::EGS_Region:		RetValue = "Region"; break;
	}

	return RetValue;
}

void FPathRequest::Load(FSaveContext& Context)
{
	// Expected Layout: (Serialized Events)
	/*
		<Portal PathID="" Scale="Location" Start="Kaedwen.KaerMorhen.GreatHall"/>
	*/

	Context.Load("PathID", mPathID);

	std::string GeoScaleString;
	Context.Load("Scale", GeoScaleString);
	mGeoScale = GetGeoScaleFromString(GeoScaleString);


	mStartPos.Load("Start", Context);
}

void FPathRequest::Save(FSaveContext& Context)
{
	Context.Save("PathID", mPathID);

	std::string GeoScaleString = GetStringFromGeoScale(mGeoScale);
	Context.Save("Save", GeoScaleString);

	mStartPos.Save("Start", Context);
}
/****************************************************************************************/

/***************************************************************************************
*  FTravelPath
****************************************************************************************/
void FTravelPath::Load(FSaveContext& Context)
{
	// Expected Layout: (Maps)
	/*
		<RavineOfTheHydra	Distance="30"	
							PointA="Kaedwen.KaerMorhen.PracticeYard"		
							PointB="Redania.TestArea.TestLoc"/>
	*/

	Context.Load("Distance", mDistance);

	mPointA.Load("PointA", Context);
	mPointB.Load("PointB", Context);
}

void FTravelPath::Save(FSaveContext& Context)
{
	Context.Save("Distance", mDistance);

	mPointA.Save("PointA", Context);
	mPointB.Save("PointB", Context);
}
/****************************************************************************************/

/***************************************************************************************
*  FLocationInfo
****************************************************************************************/
void FLocationInfo::Load(FSaveContext& Context)
{
	Context.Load("Filename", mFilename);
	Context.Load("DisplayName", mDisplayName);

	FWorldPos DefaultPos;
	FLocationInfo* PosLookup = this;
	while (PosLookup != nullptr)
	{
		if (PosLookup->mGeoScale < EGeoScale::EGS_Count)
		{
			DefaultPos[PosLookup->mGeoScale] = PosLookup->GetSaveID();
		}

		PosLookup = PosLookup->mOuter;
	}

	auto LocationFactory_Lamdba = [&](FSaveContext& InContext)
	{
		FLocationInfo* RetValue = new FLocationInfo();
		RetValue->mOuter = this;
		RetValue->mGeoScale = EGeoScale(mGeoScale - 1);
		RetValue->mPosition = DefaultPos;
		RetValue->mPosition[RetValue->mGeoScale] = InContext.GetSaveID();

		return RetValue;
	};

	if (mGeoScale == EGS_Location)
	{
		return; // Done
	}

	std::string ScaleString = "FAILURE";
	switch (mGeoScale)
	{
	case EGeoScale::EGS_Count:		ScaleString = "Regions"; break;
	case EGeoScale::EGS_Region:		ScaleString = "Areas"; break;
	case EGeoScale::EGS_Area:		ScaleString = "Locations"; break;
	case EGeoScale::EGS_Location:	ScaleString = "Wut"; break;
	}

	std::vector<FLocationInfo*> TempLocationCup;
	Context.AllocateFromWrapper(ScaleString.c_str(), TempLocationCup, LocationFactory_Lamdba);
	for (FLocationInfo* Loc : TempLocationCup)
	{
		mLocations[Loc->GetSaveID()] = Loc;
	}

	auto RoadFactory_Lamdba = [&](FSaveContext& InContext)
	{
		FTravelPath* RetValue = new FTravelPath();
		RetValue->mPointA = DefaultPos;
		RetValue->mPointB = DefaultPos;

		return RetValue;
	};

	std::vector<FTravelPath*> TempRoadmapCup;
	Context.AllocateFromWrapper("Connections", TempRoadmapCup, RoadFactory_Lamdba);
	for (FTravelPath* Road : TempRoadmapCup)
	{
		mRoads[Road->GetSaveID()] = Road;
	}
}
void FLocationInfo::Save(FSaveContext& Context)
{
	// TODO, the map... isn't it const?

	Context.Save("Filename", mFilename);
	Context.Save("DisplayName", mDisplayName);

	// TODO Locations
	// TODO Road maps
}
/****************************************************************************************/

/***************************************************************************************
*  MapUtils
****************************************************************************************/
typedef std::map<std::string, FTravelPath*>* RoadList;
typedef std::map<std::string, FLocationInfo*>* LocationList;

FTravelPath* MapUtils::GetPath(const FPathRequest& Request)
{
	FTravelPath* RetValue = nullptr;

	RoadList Roads = &(FLocationInfo::mWorldMap->mRoads);
	LocationList Locations = &(FLocationInfo::mWorldMap->mLocations);
	for (int Scale = EGeoScale::EGS_Count - 1; Scale > Request.mGeoScale; --Scale)
	{
		FLocationInfo* LocInfo = (*Locations)[Request.mStartPos[Scale]];
		Locations = &(LocInfo->mLocations);
		Roads = &(LocInfo->mRoads);
	}

	RetValue = (*Roads)[Request.mPathID];

	return RetValue;
}

FLocationInfo* MapUtils::GetLocation(const FWorldPos& Pos, EGeoScale InScale)
{
	FLocationInfo* RetValue = nullptr;

	LocationList Locations = &(FLocationInfo::mWorldMap->mLocations);
	for (int Scale = EGeoScale::EGS_Count - 1; Scale >= InScale; --Scale)
	{
		RetValue = (*Locations)[Pos[Scale]];
		Locations = &(RetValue->mLocations);
	}

	return RetValue;
}
