#ifndef COMMON_H
#define COMMON_H
#include <custom_types.h>

//Defines

#define PI 3.14159265359

//Structs

struct EXVector
{
    float x;
    float y;
    float z;
    float w;
};
typedef struct EXVector EXVector;

struct EXVector3
{
    float x;
    float y;
    float z;
};
typedef struct EXVector3 EXVector3;

struct EXVector2
{
    float x;
    float y;
};
typedef struct EXVector2 EXVector2;

struct mat44
{
    EXVector row0;
    EXVector row1;
    EXVector row2;
    EXVector row3;
};
typedef struct mat44 mat44;

struct mat33
{
    EXVector3 row0;
    EXVector3 row1;
    EXVector3 row2;
};
typedef struct mat33 mat33;

struct EXRect
{
    int x;
    int y;
    int w;
    int h;
};
typedef struct EXRect EXRect;

struct EXRectf
{
    float x;
    float y;
    float w;
    float h;
};
typedef struct EXRectf EXRectf;

struct EXPoint
{
    int x;
    int y;
};
typedef struct EXPoint EXPoint;

struct XRGBA
{
    uchar r;
    uchar g;
    uchar b;
    uchar a;
};
typedef struct XRGBA XRGBA;

struct XSprite2D
{
    XRGBA Col;
    short x;
    short y;
    ushort w;
    ushort h;
    float u1;
    float v1;
    float u2;
    float v2;
};
typedef struct XSprite2D XSprite2D;

struct EXDListItem
{
    struct EXDListItem* prev;
    struct EXDListItem* next;
};
typedef struct EXDListItem EXDListItem;

struct EXDList
{
    struct EXDListItem* head;
    struct EXDListItem* tail;
};
typedef struct EXDList EXDList;

struct EXODListItem
{
    struct EXODListItem* prev;
    struct EXODListItem* next;
    void* object;
};
typedef struct EXODListItem EXODListItem;

struct EXODList
{
    EXODListItem* head;
    EXODListItem* tail;
};
typedef struct EXODList EXODList;

struct EXCommonCamera
{
    EXRectf Rect;
    float Distance;
    float VFov;
    EXVector Position;
    EXVector Target;
    EXVector WorldUp;
    float zNear;
    float zFar;
    float zMin;
    float zMax;
    bool Perspective;
};
typedef struct EXCommonCamera EXCommonCamera;

struct EXRuntimeClass
{
    struct EXRuntimeClass* pBaseClass;
    char* pClassName;
    int ObjectSize;
    void* pCreateObject;
};
typedef struct EXRuntimeClass EXRuntimeClass;

//Enums

enum TextAlign
{
    TopLeft      = 0,
    BottomLeft   = 1,
    CentreLeft   = 2,
    TopCentre    = 3,
    Centre       = 4,
    BottomCentre = 5,
    TopRight     = 6,
    CentreRight  = 7,
    BottomRight  = 8
};
typedef enum TextAlign TextAlign;

enum Players
{
    Player_Undefined  = 0,
    Player_Spyro      = 1,
    Player_Hunter     = 2,
    Player_Sparx      = 3,
    Player_Blinky     = 4,
    Player_SgtByrd    = 5,
    Player_BallGadget = 6,
    Player_Ember      = 7,
    Player_Flame      = 8
};
typedef enum Players Players;

enum Breaths {
    Breath_Fire = 1,
    Breath_Water = 2,
    Breath_Ice = 4,
    Breath_Electric = 8
};
typedef enum Breaths Breaths;

enum MiniGameID
{
    MiniGameID_Undefined  = 0,
    MiniGameID_NoMiniGame = 1,
    MiniGameID_Cannon     = 2,
    MiniGameID_Turret     = 3,
    MiniGameID_Sparx1     = 4
};
typedef enum MiniGameID MiniGameID;

enum PStateFlags
{
    ps_none = 0x0,
    ps_willdie = 0x1,
    ps_dead = 0x2,
    ps_willwin = 0x4,
    ps_forceOff = 0x8,
    ps_Blocking = 0x10,
    ps_MiniGame = 0x20,
    ps_Shopping = 0x40,
    ps_NoObjectCollision = 0x80,
    ps_NoMoreHits = 0x100,
    ps_Locked = 0x200
};
typedef enum PStateFlags PStateFlags;

enum CamTypes
{
    Base             = 0,
    Follow           = 1,
    Scan             = 2,
    FirstPerson      = 3,
    PoleGrab         = 4,
    Fixed            = 5,
    PathFollow       = 6,
    PathDrag         = 7,
    Cutscene         = 8,
    Fall             = 9,
    cannonCam        = 10,
    BallGadgetFollow = 11,
    NPCDialog        = 12,
    Boss             = 13,
    PreviousCam      = 14,
    Tunnel           = 15,
    Rocket           = 16
};
typedef enum CamTypes CamTypes;

enum CamCreateMode
{
    SetPos   = 0,
    ForcePos = 1,
    ClonePos = 2
};
typedef enum CamCreateMode CamCreateMode;

enum PlayerModes
{
    nomode=0,
    breathe=1,
    forcebreathe=2,
    listen=3,
    listen_water=4,
    fall=5,
    fallland=6,
    falllongland=7,
    jump=8,
    swampjump=9,
    jumpland=10,
    doublejump=11,
    jumpledgegrab=12,
    jumpslam=13,
    jumpslamland=14,
    scan=15,
    walk=16,
    charge=17,
    supercharge=18,
    glide=19,
    glideland=20,
    glidestart=21,
    walkstop=22,
    walkturn180=23,
    hitwall=24,
    hitwallwater=25,
    breatheFire=26,
    tailspin=27,
    jumpcharge=28,
    chargepunch=29,
    polegrab=30,
    polegrabslip=31,
    firstperson=32,
    firstpersonwater=33,
    takedamage=34,
    death=35,
    iceydeath=36,
    teeter=37,
    collidebounce=38,
    spit=39,
    deathfall=40,
    teleportIn=41,
    teleportOut=42,
    idleSparx=43,
    wallkick=44,
    wallkickledgegrab=45,
    stunned=46,
    sneak=47,
    water_paddle=48,
    water_dive=49,
    water_jump=50,
    water_surface=51,
    water_hitswim=52,
    water_hitpaddle=53,
    water_death=54,
    fire_cannon=55,
    fire_turret=56,
    fire_cannonMini=57,
    thermal_glide=58,
    swamp_death=59,
    eaten=60,
    track2point=61,
    swim2point=62,
    followpath=63,
    bombing=64,
    wingshield=65,
    roofhang=66,
    squashed=67,
    throwbomb=68,
    shootgun=69,
    digwall=70,
    digfloor=71,
    crouch=72,
    drown=73,
    shopping=74,
    frozen=75,
    forceturn=76,
    lockpick=77,
    hunter_firebow=78,
    hunter_jumpattack=79,
    hunter_dbljumpattack=80,
    hunter_dashpunch=81,
    hunter_defend=82,
    hunter_slidekick=83,
    hunter_roundhousekick=84,
    hunter_backflipkick=85,
    hunter_hide=86,
    hunter_wallclimb=87,
    hunter_wallclimbjump=88,
    hunter_stompkick=89,
    hunter_uppercut=90,
    hunter_ledgecling=91,
    hunter_tarBreathe=92,
    hunter_tarLand=93,
    hunter_tarWalk=94,
    hunter_tarJump=95,
    hunter_tarPunch=96,
    hunter_tarShoot=97,
    hunter_tarDefend=98,
    hunter_tarFirstPerson=99,
    hunter_tarHit=100,
    hunter_climbhit=101,
    sparx_flytunnel=102,
    spark_heldinweb=103,
    endjump=104,
    icejump=105
};
typedef enum PlayerModes PlayerModes;

enum Modes_Sparx {
    spx_idle1 = 0x0,
    spx_idle2 = 0x1,
    spx_idle3 = 0x2,
    spx_follow = 0x3,
    spx_returning = 0x4,
    spx_collectingGem = 0x5,
    spx_chasingFodder = 0x6,
    spx_hiding = 0x7,
    spx_spyroIdle = 0x8,
    spx_spyrochat = 0x9,
    spx_pathfollow = 0xa,
    spx_lockpick = 0xb,
    spx_fullhealth = 0xc
};
typedef enum Modes_Sparx Modes_Sparx;

enum PlayerAbilities
{
    Abi_DoubleJump = 0x1,
    //Abi_UNK0 = 0x2,
    Abi_HitPointUpgrade = 0x4,
    //Abi_UNK1 = 0x8,
    Abi_PoleSpin = 0x10,
    Abi_IceBreath = 0x20,
    Abi_ElectricBreath = 0x40,
    Abi_WaterBreath = 0x80,
    //Abi_UNK2 = 0x100,
    Abi_DoubleGem = 0x200,
    //Abi_UNK3 = 0x400,
    Abi_Aqualung = 0x800,
    Abi_SuperCharge = 0x1000,
    Abi_Invincibility = 0x2000,
    Abi_ShopSellsEverything = 0x4000,
    Abi_WingShield = 0x8000,
    Abi_WallKick = 0x10000,
    Abi_HornDiveUpgrade = 0x20000,
    Abi_ButterflyJar = 0x40000,
};
typedef enum PlayerAbilities PlayerAbilities;

//Buttons

enum Buttons
{
    Button_Dpad_Up    = 0x1,
    Button_Dpad_Down  = 0x2,
    Button_Dpad_Left  = 0x4,
    Button_Dpad_Right = 0x8,

    Button_LAnalog_Up    = 0x10,
    Button_LAnalog_Down  = 0x20,
    Button_LAnalog_Left  = 0x40,
    Button_LAnalog_Right = 0x80,

    Button_RAnalog_Up    = 0x100,
    Button_RAnalog_Down  = 0x200,
    Button_RAnalog_Left  = 0x400,
    Button_RAnalog_Right = 0x800,

    Button_B = 0x1000,
    Button_A = 0x2000,
    Button_X = 0x4000,
    Button_Y = 0x8000,

    Button_L3_Unused = 0x10000,
    Button_R3_Unused = 0x20000,
    Button_Start     = 0x40000,
    Button_Select    = 0x80000,

    Button_L         = 0x100000,
    Button_L2_Unused = 0x200000,
    Button_R         = 0x400000,
    Button_Z         = 0x800000
};
typedef enum Buttons Buttons;

enum BreathType
{
    fire = 0x1,
    water = 0x2,
    ice = 0x4,
    elec = 0x8,
    all_types = 0x3f
};
typedef enum BreathType BreathType;

struct Analog
{
    float LStick_X;
    float LStick_Y;
    float RStick_X;
    float RStick_Y;
    int dat1[8];
    float LTrigger;
    int dat2;
    float RTrigger;
    int dat3;
};
typedef struct Analog Analog;

//Constants



#endif /* COMMON_H */