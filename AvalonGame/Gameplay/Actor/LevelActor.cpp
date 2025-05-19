/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "LevelActor.h"
#include "../GameTime/GameCalendar.h"
#include "../Components/ContainerComponent.h"
#include "../Map/TravelTypes.h"

LevelActor* LevelActor::mCurrentLevel = nullptr;
LevelActor* LevelActor::mStreamingLevel = nullptr;

/***************************************************************************************
*  Level Actor
****************************************************************************************/
LevelActor::LevelActor()
{
    SetSavesToFile();

}

LevelActor::~LevelActor()
{

}

void LevelActor::Tick(float DeltaSeconds)
{
    if (mLevelContainer)
    {
        mLevelContainer->Tick(DeltaSeconds);
    }
}

void LevelActor::AdvanceTime(long DeltaHours)
{
    if (mLevelContainer)
    {
        mLevelContainer->AdvanceTime(DeltaHours);
    }
}

void LevelActor::OnLevelOpened()
{
    ISaveable::LoadFromFile(this, mLocation->mFilename);
    InitializeDefaultComponents();

    GameCalendar::GameTimeEvent::Callback Callback = LevelActor::HandleTimeAdvanced;
    GameCalendar::BindEvent_TimeAdvanced(this, Callback);
}

void LevelActor::OnLevelClosed()
{
    GameCalendar::UnBindEvent_TimeAdvanced(this);

    if (mLocation)
    {
        ISaveable::SaveToFile(this, mLocation->mFilename);
    }
}

FUnitHandle LevelActor::LoadActor(const FUnitHandle& Owner, FSaveContext& Context)
{
    FUnitHandle NewHandle = AvalonMemory::NewUnit<AvalonActor>();
    AvalonActor* NewActor = NewHandle.Get<AvalonActor>();
    NewActor->SetOwner(Owner);

    if (AvalonActor* Outer = Owner.Get<AvalonActor>())
    {
        NewActor->SetLocation(Outer->GetLocation());
        NewActor->mLevel    = Outer->mLevel;
    }

    ISaveable::Load(NewActor, Context);

    return NewHandle;
}

/*static*/ void LevelActor::OpenLevel(LevelActor*& Level, FLocationInfo* NewLocation)
{
    // We are closing a level before opening one
    if (Level != nullptr)
    {
        // Unbinds events, and saves the level file
        Level->OnLevelClosed();

        // TODO:  Evaluate if need to do more for "unloading"
        AvalonMemory::DestroyUnit(Level);
    }

    const FUnitHandle LevelHandle = AvalonMemory::NewUnit<LevelActor>();

    Level = LevelHandle.Get<LevelActor>();
    Level->mLocation = NewLocation;
    Level->mLevel    = Level->GetSelfHandle();

    // Initializes components and loads from file
    Level->OnLevelOpened();
}

/*static*/ void LevelActor::PlaceActorInLevel(FUnitHandle& ActorHandle, LevelActor* NewLevel)
{
    AvalonActor* Actor = ActorHandle.Get<AvalonActor>();
    Actor->SetLocation(NewLevel->mLocation);
    Actor->mLevel = NewLevel->GetSelfHandle();

    if (LevelContainer* Container = NewLevel->mLevelContainer)
    {
        Container->AddToContainer(ActorHandle);
    }
}

/*static*/ void LevelActor::MoveActorTo(FUnitHandle& ActorHandle, LevelActor* NewLevel)
{
    AvalonActor* Actor = ActorHandle.Get<AvalonActor>();

    // Detach it from its current level:
    LevelActor* From = Actor->mLevel.Get<LevelActor>();
    if (LevelContainer* Container = From->mLevelContainer)
    {
        // TODO:  What if this is inside of something?  Or equipment?
        Container->RemoveFromContainer(ActorHandle);
    }

    PlaceActorInLevel(ActorHandle, NewLevel);
}

/*static*/ void LevelActor::HandleTimeAdvanced(IEventListener* Listener, long HoursAdvanced)
{
    LevelActor* Level = static_cast<LevelActor*>(Listener);
    Level->AdvanceTime(HoursAdvanced);
}

void LevelActor::GetActorsPlacedInLevel(std::vector<FUnitHandle>& OutActors) const
{
    if (mLevelContainer)
    {
        mLevelContainer->GetContainerContents(OutActors);
    }
}

void LevelActor::InitializeDefaultComponents()
{
    // Look for the floor container
    std::vector<FUnitHandle> ActorsInLevel;
    GetActorsPlacedInLevel(ActorsInLevel);

    for (FUnitHandle& ActorHandle : ActorsInLevel)
    {
        if (AvalonActor* Actor = ActorHandle.Get<AvalonActor>())
        {
            if (Actor->mDisplayName == "The Floor")
            {
                mFloorContainer = Actor->GetComponent<IContainer>();
                return;
            }
        }
    }

    // TODO:  Make the "Floor" unsavable
    //  Things "on the floor" are Lootables?
}

/***************************************************************************************
*  IActorComponent
****************************************************************************************/
FUnitHandle IActorComponent::LoadChildActor(FSaveContext& Context)
{
    FUnitHandle RetValue;

    if (Context.IsValid())
    {
        FUnitHandle& OwnerHandle = GetActorOwnerHandle();
        if (AvalonActor* Outer = OwnerHandle.Get<AvalonActor>())
        {
            if (LevelActor* Level = Outer->mLevel.Get<LevelActor>())
            {
                RetValue = Level->LoadActor(OwnerHandle, Context);
            }
            else if (LevelActor* Level = dynamic_cast<LevelActor*>(Outer))
            {
                RetValue = Level->LoadActor(OwnerHandle, Context);
            }
        }
    }

    return RetValue;
}
