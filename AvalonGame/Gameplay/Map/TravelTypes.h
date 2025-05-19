/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "../Utility/Saveable.h"

#include <string>
#include <map>

enum EGeoScale
{
    EGS_Location,
    EGS_Area,
    EGS_Region,
    EGS_Count
};

struct FWorldPos
{
public:
    void Load(const char* ID, FSaveContext& Context);
    void Save(const char* ID, FSaveContext& Context);

    const std::string& operator[](unsigned int index) const;
    std::string& operator[](unsigned int index);

    FWorldPos& operator=(const FWorldPos& other);

    friend bool operator==(const FWorldPos& lhs, const FWorldPos& rhs);

    bool IsValid() const;

private:
    std::string mPos[EGeoScale::EGS_Count];
};

struct FPathRequest
{
    void Load(FSaveContext& Context);
    void Save(FSaveContext& Context);

    std::string mPathID;
    EGeoScale mGeoScale = EGeoScale::EGS_Location;
    FWorldPos mStartPos;
};

// TODO : Rename FTravelPath to FTravelLeg for clarity (differentiate from FPathRequest)
struct FTravelPath : public ISaveable
{
    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    FWorldPos mPointA;
    FWorldPos mPointB;
    float mDistance = 0.f;
};

struct FLocationInfo : public ISaveable
{
    static FLocationInfo* mWorldMap;

    void Load(FSaveContext& Context) override;
    void Save(FSaveContext& Context) override;

    FLocationInfo* mOuter;

    EGeoScale mGeoScale = EGeoScale::EGS_Count;
    FWorldPos mPosition;

    std::string mFilename;
    std::string mDisplayName;

    std::map<std::string, FTravelPath*> mRoads;
    std::map<std::string, FLocationInfo*> mLocations;
};

namespace MapUtils
{
    FTravelPath* GetPath(const FPathRequest& Request);
    FLocationInfo* GetLocation(const FWorldPos& Pos, EGeoScale InScale);
}


