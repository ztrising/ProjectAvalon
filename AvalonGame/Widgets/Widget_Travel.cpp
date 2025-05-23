/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

#include "Widget_Travel.h"

#include "../Gameplay/Actor/PlayerActor.h"
#include "../Gameplay/Components/TravellerComponent.h"
#include "../Gameplay/AvalonGameState.h"

#include <string>

Widget_Travel::Widget_Travel()
{
	mValidHUDContexts = EHUDContext::LOCATION | EHUDContext::TRAVELLING;
}

Widget_Travel::~Widget_Travel()
{
	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	if (Player != nullptr)
	{
		if (Traveller* TravellerComp = Player->GetComponent<Traveller>())
		{
			TravellerComp->mOnTravelBegin.UnBindEvent(this);
			TravellerComp->mOnTravelEnd.UnBindEvent(this);
		}
	}
}

typedef std::string LocationNameList[EGeoScale::EGS_Count];

void PopulateLocationName(const FLocationInfo* Info, LocationNameList& GeoNames)
{
	if (Info && Info->mGeoScale < EGeoScale::EGS_Count && Info->mGeoScale >= 0)
	{
		GeoNames[Info->mGeoScale] = Info->mDisplayName;
		PopulateLocationName(Info->mOuter, GeoNames);
	}
}

void Widget_Travel::PopulateTravelText()
{
	////////////////////////////////////
	//                                //
	//  Region: Kaedwen               //
	//  Area:	Kaer Morhen			  //
	//								  //
	////////////////////////////////////
	//                                //
	//			Travelling			  //
	//			   via				  //
	//			 Archway		      //
	//                                //
	//		Kitchen --> GreatHall	  //
	//                                //
	//		  Journey Progress		  //
	// [|||||||||| <50%>            ] //
	//                                //
	////////////////////////////////////

	////////////////////////////////////
	//                                //
	//  Region: Kaedwen               //
	//  Area:	Kaer Morhen			  //
	//								  //
	////////////////////////////////////
	//								  //
	//	Location:	GreatHall		  // 
	//								  //
	////////////////////////////////////

	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	FLocationInfo* LocationInfo = Player->GetLocation();
	LocationNameList GeoNames;
	PopulateLocationName(LocationInfo, GeoNames);

	Traveller* TravellerComp = Player->GetComponent<Traveller>();

	FTextSettings TextSettings;
	TextSettings.mExtent		= FCoord(20, 1);
	TextSettings.mFillChar		= ' ';
	TextSettings.mFillExtent	= true;
	TextSettings.mJustified		= ETextJustification::Left;
	TextSettings.mOffset		= FCoord(2, 2);

	TextSettings.mUseBufferExtentHeight = false;
	TextSettings.mUseBufferExtentWidth	= false;

	{
		////////////////////////////////////
		//                                //
		//  Region: Kaedwen               //
		//  Area:	Kaer Morhen			  //
		//								  //
		////////////////////////////////////

		TextSettings.mOffset		= FCoord(4, 2);
		std::string RegionString	= GeoNames[EGeoScale::EGS_Region].c_str();
		std::string BoxString		= "Region: " + RegionString;
		SetTextWithSettings(BoxString.c_str(), TextSettings);

		TextSettings.mOffset		= FCoord(4, 3);
		std::string AreaString		= GeoNames[EGeoScale::EGS_Area].c_str();
		BoxString					= "Area:   " + AreaString;
		SetTextWithSettings(BoxString.c_str(), TextSettings);
	}

	if (TravellerComp->IsTravelling())
	{
		////////////////////////////////////
		//                                //
		//			Travelling			  //
		//			   via				  //
		//			 Archway		      //
		//                                //
		//		Kitchen --> GreatHall	  //
		//                                //
		//		  Journey Progress		  //
		// [|||||||||| <50%>            ] //
		//                                //
		////////////////////////////////////

		TextSettings.mExtent = FCoord(33, 1);
		TextSettings.mJustified = ETextJustification::Center;
		TextSettings.mOffset = FCoord(2, 5);

		FTravelPath* Path = TravellerComp->GetPath();
		const FPathRequest& PathRequest = TravellerComp->GetPathRequest();

		std::string BoxString = "Travelling via " + PathRequest.mPathID;
		SetTextWithSettings(BoxString.c_str(), TextSettings);

		BoxString = PathRequest.mStartPos[EGeoScale::EGS_Location];
		BoxString = BoxString + " --> ";

		if (PathRequest.mStartPos == Path->mPointA)
		{
			BoxString = BoxString + Path->mPointB[EGeoScale::EGS_Location];
		}
		else
		{
			BoxString = BoxString + Path->mPointA[EGeoScale::EGS_Location];
		}

		TextSettings.mOffset = FCoord(2, 6);	// TODO
		SetTextWithSettings(BoxString.c_str(), TextSettings);

		// TODO:  Progress Bar!
	}
	else
	{
		////////////////////////////////////
		//								  //
		//	Location:	GreatHall		  // 
		//								  //
		////////////////////////////////////

		TextSettings.mExtent	= FCoord(33, 1);
		TextSettings.mJustified = ETextJustification::Center;

		TextSettings.mOffset = FCoord(2, 5);
		std::string BoxString = "Current Location:";

		SetTextWithSettings(BoxString.c_str(), TextSettings);

		TextSettings.mOffset = FCoord(2, 6);
		BoxString = GeoNames[EGeoScale::EGS_Area].c_str();

		SetTextWithSettings(BoxString.c_str(), TextSettings);
	}
}

/***************************************************************************************
*  AvalonWidget
****************************************************************************************/
void Widget_Travel::Construct(const char* WidgetAsset)
{
	AvalonWidget::Construct("W_TravelStatus.xml");
}

void Widget_Travel::OnGameLoaded()
{
	AvalonActor* Player = AvalonGameState::GetPlayerActor();
	if (Player != nullptr)
	{
		if (Traveller* TravellerComp = Player->GetComponent<Traveller>())
		{
			Traveller::TravelEvent::Callback Callback = Widget_Travel::OnStartTravel;
			TravellerComp->mOnTravelBegin.BindEvent(this, Callback);

			Callback = Widget_Travel::OnFinishTravel;
			TravellerComp->mOnTravelEnd.BindEvent(this, Callback);
		}
	}

	PopulateTravelText();
}
/****************************************************************************************/

/***************************************************************************************
*  IEventListener
****************************************************************************************/
/*static*/ void Widget_Travel::OnStartTravel( IEventListener* Listener
											, Traveller* InTraveller)
{
	Widget_Travel* Widget = static_cast<Widget_Travel*>(Listener);
	Widget->PopulateTravelText();
}

/*static*/ void Widget_Travel::OnFinishTravel( IEventListener* Listener
											 , Traveller* InTraveller)
{
	Widget_Travel* Widget = static_cast<Widget_Travel*>(Listener);
	Widget->PopulateTravelText();
}
/****************************************************************************************/
