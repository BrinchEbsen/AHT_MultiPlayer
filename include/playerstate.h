#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H
#include <custom_types.h>
#include <common.h>

/*
PlayerState struct - stores general information about the player.
Field names are from the original code (minus the "m_" prefix).
*/

struct PlayerSetupInfo
{
    EXVector Position;
    EXVector Rotation;
    Players Player;
    MiniGameID MiniGameID;
    int MapListIndex;
    CamTypes CameraType;
    CamCreateMode CameraCreateMode;
    uint Flags;
};
typedef struct PlayerSetupInfo PlayerSetupInfo;

struct PlayerState
{
    int CurrentBreath;
    int Health;
    int Gems;
    int TotalGems;
    byte LockPickers;
    byte LockPickers_Max;
    byte LockPickers_Total;
    byte LockPickers_Magazines;
    byte FlameBombs;
    byte FlameBombs_Max;
    byte FlameBombs_Total;
    byte FlameBombs_Magazines;
    byte IceBombs;
    byte IceBombs_Max;
    byte IceBombs_Total;
    byte IceBombs_Magazines;
    byte WaterBombs;
    byte WaterBombs_Max;
    byte WaterBombs_Total;
    byte WaterBombs_Magazines;
    byte ElectricBombs;
    byte ElectricBombs_Max;
    byte ElectricBombs_Total;
    byte ElectricBombs_Magazines;
    short Xarrows;
    short Xarrows_Max;
    PlayerAbilities AbilityFlags;
    float WaterDiveTimer; //Unused
    float SuperchargeTimer;
    float SuperchargeTimerMax;
    float InvincibleTimer;
    float InvincibleTimerMax;
    float DoubleGemTimer;
    float DoubleGemTimerMax;
    float SgtBird_Fuel;
    short SgtBird_Bombs;
    short SgtBird_Missiles;
    short SparxSmartBombs;
    short SparxSeekers;
    short BlinkBombs;
    byte TotalLightGems;
    byte TotalDarkGems;
    byte TotalDragonEggs;
    byte padding[7];
    PlayerSetupInfo Setup;
    Players LastPlayerSetup;
};
typedef struct PlayerState PlayerState;

#endif /* PLAYERSTATE_H */