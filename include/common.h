#ifndef COMMON_H
#define COMMON_H
#include <custom_types.h>

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

struct XRGBA
{
    uchar r;
    uchar g;
    uchar b;
    uchar a;
};
typedef struct XRGBA XRGBA;

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

extern XRGBA COLOR_TEXT;
extern XRGBA COLOR_WHITE;
extern XRGBA COLOR_BLACK;
extern XRGBA COLOR_RED;
extern XRGBA COLOR_GREEN;
extern XRGBA COLOR_BLUE;
extern XRGBA COLOR_LIGHT_RED;
extern XRGBA COLOR_LIGHT_GREEN;
extern XRGBA COLOR_LIGHT_BLUE;

#endif /* COMMON_H */