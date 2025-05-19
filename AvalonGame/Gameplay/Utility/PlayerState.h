/***************************************************************************************
*  Project Avalon - 2022 - Amy Price
****************************************************************************************/

#pragma once

/*class PlayerState : public IGameTimeListener
{
public:
    void LoadFromSave(const char* SaveFile);
    void Tick(float DeltaTime);

    void DealDamageToStat(EPlayerStats eStat, float Damage);

    FPlayerStat mStats[EPlayerStats::EPlayerStatsCount];

    friend void ListenForStatValueChanges(IPlayerStatListener* Listener);
    friend void UnlistenForStatValueChanges(IPlayerStatListener* Listener);

private:
    DynamicArray<IPlayerStatListener*> mListeners;


public:
    void HandleTimeAdvanced(const FGameTime& CurrentTime, const FGameTime& TimeDelta) override;

    
    //////////////////////////////////////////////////////////////////////////
    //  Singleton Implementation
public:
    static PlayerState& GetPlayerState()
    {
        static PlayerState State;
        return State;
    }

    PlayerState(PlayerState const&) = delete;
    void operator=(PlayerState const&) = delete;

private:
    PlayerState() {}
    ~PlayerState() {};
    //////////////////////////////////////////////////////////////////////////
};*/
