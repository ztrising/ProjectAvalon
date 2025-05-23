/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "PlayerActor.h"
#include "LevelActor.h"

#include "../Components/ContainerComponent.h"


/***************************************************************************************
*  Player Actor - The Player!
****************************************************************************************/
PlayerActor::PlayerActor()
{
    SetSavesToFile();
}

/*static*/ void PlayerActor::LoadPlayer()
{
	/*PlayerActor::mPlayerHandle = AvalonMemory::NewUnit<PlayerActor>();
	PlayerActor::mPlayer		= mPlayerHandle.Get<PlayerActor>();
	
	LevelActor::PlaceActorInLevel(mPlayerHandle, LevelActor::mCurrentLevel);

	ISaveable::LoadFromFile(PlayerActor::mPlayer, "PlayerActor.xml");*/
}

/*static*/ void PlayerActor::SavePlayer()
{
	//ISaveable::SaveToFile(PlayerActor::mPlayer, "PlayerActor.xml");
}
