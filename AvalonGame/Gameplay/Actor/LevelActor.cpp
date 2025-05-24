/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#include "LevelActor.h"
#include "../GameTime/GameCalendar.h"
#include "../Components/ContainerComponent.h"
#include "../Map/TravelTypes.h"

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

void LevelActor::LoadActor(HardUnitRef& OutNewActor, HardUnitRef& OwnerRef, FSaveContext& Context)
{
    AvalonMemory::NewUnit<AvalonActor>(OutNewActor);
    
    AvalonActor* NewActor = Get<AvalonActor>(OutNewActor);
    if (NewActor != nullptr)
    {
        NewActor->SetOwner(OwnerRef);

        AvalonActor* OwnerActor = Get<AvalonActor>(OwnerRef);
        if (OwnerActor != nullptr)
        {
            NewActor->SetLocation(OwnerActor->GetLocation());
            NewActor->mLevelRef = GetSelfRef();// OwnerActor->mLevelRef;
        }
    }
    
    ISaveable::Load(NewActor, Context);
}

/*static*/ void LevelActor::PlaceActorInLevel(HardUnitRef& ActorRef, LevelActor* Level)
{
    if (Level != nullptr)
    {
        AvalonActor* Actor = Get<AvalonActor>(ActorRef);
        if (Actor != nullptr)
        {
            Actor->SetLocation(Level->mLocation);
            Actor->mLevelRef = Level->GetSelfRef();

            if (LevelContainer* Container = Level->mLevelContainer)
            {
                Container->AddToContainer(Actor);
            }
        }
    }
}

/*static*/ void LevelActor::RemoveActorFromLevel(HardUnitRef& ActorRef, LevelActor* Level)
{
    if (LevelContainer* Container = Level->mLevelContainer)
    {
        Container->RemoveFromContainer(ActorRef);
    }
}

/*static*/ void LevelActor::MoveActorTo(HardUnitRef& ActorRef, LevelActor* Level)
{
    if (Level != nullptr)
    {
        AvalonActor* Actor = Get<AvalonActor>(ActorRef);
        if (Actor != nullptr)
        {
            LevelActor* PrevLevel = Get<LevelActor>(Actor->mLevelRef);
            if (PrevLevel != nullptr)
            {
                // TODO:  What if this is inside of something?  Or equipment?
                RemoveActorFromLevel(ActorRef, PrevLevel);
            }

            PlaceActorInLevel(ActorRef, Level);
        }
    }
}

/*static*/ void LevelActor::HandleTimeAdvanced(IAvalonUnit* Listener, long HoursAdvanced)
{
    LevelActor* Level = static_cast<LevelActor*>(Listener);
    Level->AdvanceTime(HoursAdvanced);
}

void LevelActor::GetActorsPlacedInLevel(HardRefList& OutActors) const
{
    if (mLevelContainer)
    {
        mLevelContainer->GetContainerContents(OutActors);
    }
}

void LevelActor::InitializeDefaultComponents()
{
    // Look for the floor container
    HardRefList ActorsInLevel;
    GetActorsPlacedInLevel(ActorsInLevel);

    for (HardUnitRef& ActorRef : ActorsInLevel)
    {
        if (AvalonActor* Actor = Get<AvalonActor>(ActorRef))
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
void IActorComponent::LoadChildActor(HardUnitRef& OutChildActor, FSaveContext& Context)
{
    if (Context.IsValid())
    {
        AvalonActor* OwnerActor = GetActorOwner();
        if (OwnerActor != nullptr)
        {
            HardUnitRef OwnerRef = OwnerActor->GetSelfRef();

            LevelActor* Level = Get<LevelActor>(OwnerActor->mLevelRef);
            if (Level != nullptr)
            {
                HardUnitRef LevelRef = OwnerActor->mLevelRef.lock();
                Level->LoadActor(OutChildActor, OwnerRef, Context);
            }
            else
            {
                if (LevelActor* OwnerActorLevel = dynamic_cast<LevelActor*>(OwnerActor))
                {
                    OwnerActorLevel->LoadActor(OutChildActor, OwnerRef, Context);
                }
                else
                {
                    int i = 0;
                    // something went terribly wrong
                }
            }
        }
    }
}
