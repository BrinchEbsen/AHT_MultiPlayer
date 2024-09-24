#ifndef SYMBOLS_H
#define SYMBOLS_H
#include <custom_types.h>
#include <common.h>
#include <buttons.h>
#include <playerstate.h>

/*
Pointers to complex structs have been made int pointers by default.

In function signatures that feature a this-pointer that is just a global struct, the first argument will be
the global struct name prefixed with an underscore.
*/

//Globals:

//Player handler
in_game int* gpPlayer; //0x804CB35C
//Player item
in_game int* gpPlayerItem; //0x804CB360
//Sparx handler
in_game int* gpSparx; //0x804cb364
//Game (World) window
in_game int* gpGameWnd; //0x804cb3cc
//Panel (HUD) window
in_game int* gpPanelWnd; //0x804cb3d0
//Text strings object
in_game int* gGameText; //0x80455c1c
//Game counter (doesn't count during pause)
in_game int gGameCounter; //0x804cb64c
//Rand class (only 1 field)
in_game uint g_EXRandClass; //0x804cc854
//Current pad in use (0 to 3)
in_game int g_PadNum; //0x804cb660
//Player state class (health, gems etc...)
in_game PlayerState gPlayerState; //0x80465B60
//Item environment
in_game int theItemEnv; //0x8046f5f0
//List of game items
in_game EXDList* ItemEnv_ItemList; //0x8046F670
//Amount of game items
in_game int ItemEnv_ItemCount; //0x8046F6AC
//Game Loop
in_game int gGameLoop; //0x8046f2f0
//Global camera struct
in_game EXCommonCamera gCommonCamera; //0x80750860

//Buttons
in_game uint Pads_ButtonDown[4]; //0x8066AAD0
in_game uint Pads_ButtonLast[4]; //0x8066AAF0
in_game uint Pads_ButtonPressed[4]; //0x8066AB10
in_game uint Pads_ButtonRelease[4]; //0x8066AB30
//Analog
in_game Analog Pads_Analog[4]; //0x8066AB50

in_game mat44 CamMatrix; //0x8048A1A0

//Standard C/C++ library functions that already exist in the code. Prefixed with "ig_" to avoid issues.
//printf outputs to Dolphin's log window.

//Memory (in most cases it's probably preferable to use EX's proprietary EXAlloc and EXFree functions):
in_game void* ig_malloc(size_t size); //0x80259cc4
in_game void* ig_memcpy(void* dest, void* source, size_t num); //0x80259d00
in_game void* ig_memmove(void* dest, void* source, size_t num); //0x80259da4
in_game void* ig_memset(void* ptr, int value, size_t num); //0x80259e90
//Note: free was overwritten by EXFree

//Strings:
in_game int ig_printf(char* format, ...); //0x803592a8
in_game int ig_sprintf(char* str, char* format, ...); //0x80259af8
in_game int ig_vsprintf(char* str, char* format, va_list arg); //0x80259bd0
in_game char* ig_strcat(char* dest, char* source); //0x80359da4
in_game int ig_strcasecmp(char* str1, char* str2); //0x80259f24
in_game int ig_strcmp(char* str1, char* str2); //0x80259fa0
in_game char* ig_strcpy(char* dest, char* source); //0x8025a048
in_game size_t ig_strlen(char* str); //0x8025a0cc
in_game char* ig_strncopy(char* dest, char* source, size_t count); //0x8025a138
in_game char* ig_strupr(char* str); //0x8025a204
in_game char* ig_strrev(char* str); //0x8025a2cc
//For turning a recular char string into a wchar (16 bit) string
in_game wchar_t* ig_wcscpy(wchar_t* pDest, char* pSource); //0x802fbe30

//Float math:
in_game float ig_powf(float base, double exp); //0x80257c44
in_game float ig_sqrtf(float x); //0x80258338
in_game float ig_fmodf(float x, float y); //0x80257a7c
in_game float ig_fabsf(float x); //0x802587e0
in_game float ig_floorf(float x); //0x80258804
in_game float ig_ceilf(float x); //0x80258640
in_game float ig_sinf(float x); //0x802588cc
in_game float ig_cosf(float x); //0x80258704
in_game float ig_tanf(float x); //0x802589a8
in_game float ig_asinf(float x); //0x80257630
in_game float ig_acosf(float x); //0x802573e4
in_game float ig_atanf(float x); //0x80258430
in_game float ig_atan2f(float y, float x); //0x8025785c

//EX-specific functions

//Allocate memory on the heap. memflags can usually be left as 0
in_game void* EXAlloc(size_t size, uint memflags); //0x802caf50
//Free memory from the heap
in_game void EXFree(void* pData); //0x802cb080

in_game int* _SystemHeapList; //0x80485b38

//Set the text properties. This should be done before printing anything.
//File should be set to HT_File_Panel. Font is usually HT_Font_Test.
in_game void XWnd_SetText(int* _gpPanelWnd, long File, long Font, XRGBA* Col, float Scale, TextAlign Align); //0x8012f8dc
//Draw text to the screen.
in_game void XWnd_FontPrint (int* _gpPanelWnd, u16 x, u16 y, char* pText,   float Scale, TextAlign Align, bool Filter); //0x80134a34
//Draw text to the screen (16bit string).
in_game void XWnd_FontPrintW(int* _gpPanelWnd, u16 x, u16 y, wchar_t* pText, float Scale, TextAlign Align, bool Filter); //0x801335fc
//Draw text to the screen (hashcode).
in_game void XWnd_TextPrint(int* self, uint hash); //0x8012f6e0

//Get screen coordinates from a world position
in_game EXVector2* WorldToDisp(EXVector2* dest, EXVector* vct); //0x802807a8

//Draw a rectangle to the screen.
in_game void Util_DrawRect(int* _gpPanelWnd, EXRect *Rect, XRGBA *Col); //0x801aacc4
in_game void Util_DrawSector(EXPoint* point, float innerRadius, float outerRadius, float startAng, float endAng, XRGBA* c1, XRGBA* c2, XRGBA* c3, XRGBA* c4, float resolution, int* pWnd); //0x802513bc
in_game int* Util_GetGameInfoTexture(uint Hash); //0x802506b4

//Get reference to the map the player is in. FindFlag is usually left as 0.
in_game int* GetSpyroMap(long FindFlag); //0x80232df0

in_game void GameSetPauseOn(int* _gGameLoop, int PanelFlag); //0x802400c8
in_game void GameSetPauseOff(int* _gGameLoop, int PanelFlag); //0x80240164

//Get any text string from the game referenced by hashcode (HT_Text_*).
//Return type is a 16-bit wchar*.
//pWho and Color can be left as 0.
in_game wchar_t* GetText(int* _gGameText, long textHash, long pWho, int Color); //0x8012df2c

//Play an SFX using a hashcode (HT_SFX_*)
//This only works if the SFX is loaded in a soundbank. Generic and streamed sounds are always available.
in_game void PlaySFX(long hash); //0x8024bc58

//Random Number Generation
in_game uint Rand32(uint* _g_EXRandClass); //0x802f50a8
in_game float Randf(uint* _g_EXRandClass); //0x802f50e0
in_game void RandSetSeed(uint* _g_EXRandClass, uint set); //0x802f5088

in_game void EXRect_Copy(EXRect* self, EXRect* other); //0x803881c0
in_game void EXPoint_Copy(EXPoint* self, EXPoint* other); //0x803881a8

//The v_PlayerSetup method on the SE_Map class.
//Can be overridden for whatever specific setups a map might have.
typedef EXVector* (*SE_Map_v_PlayerSetup)(int*, EXVector*, EXVector*);

//The v_StateRunning method on the GUI_PanelItem class.
//It handles the main drawing logic for the HUD elements.
typedef int (*GUI_PanelItem_v_StateRunning_func)(int*);
//v_StateRunning vtable entry for GUI_PanelItem
in_game GUI_PanelItem_v_StateRunning_func vtable_GUI_PanelItem_v_StateRunning; //0x80445d28
in_game int GUI_PanelItem_v_StateRunning(int* self); //0x8021eb3c

//The v_DrawStateRunning method on the GUI_PauseMenu class.
typedef int (*GUI_PauseMenu_v_DrawStateRunning_func)(int*, int*);
//v_DrawStateRunning vtable entry for GUI_PauseMenu
in_game GUI_PauseMenu_v_DrawStateRunning_func vtable_GUI_PauseMenu_v_DrawStateRunning; //0x8042d968
in_game int GUI_PauseMenu_v_DrawStateRunning(int* self, int* pWnd); //0x8039874c

typedef void (*LoadingLoopDraw_func)(int*, int*);
in_game LoadingLoopDraw_func vtable_LoadingLoopDraw; //0x80446370

//The Delete method on the XSEItemHandler class.
//NOTE: The player subclass implementations also set the global references to NULL!
typedef int (*ItemHandler_Delete)(int*);

//The DoItemPhysics method on the EXItem class (and its derivitives).
typedef void (*EXItem_DoItemPhysics)(int*);

//Generic GetRuntimeClass function
typedef EXRuntimeClass* (*GetRuntimeClass_func)(void);

//The SetMiniGameDie method on the SEMap_Minigame class
typedef void (*SE_Map_SetMiniGameDie_func)(int*);

in_game int SE_GameLoop_State; //0x8046F344
in_game float Camera_Follow_Elevation_Default; //0x803bf66c
in_game float Camera_BallGadgetFollow_Distance_UpperLimit; //0x803bf070

in_game bool ItemHandler_SEKill(int* self); //0x80242210

in_game void XSEItemEnv_RemoveXSEItemHandler(int* _theItemEnv, int* pHandler); //0x80240a8c

in_game void SE_Trigger_DoKill(int* self); //0x8023a9d0

in_game int gPlayerLoader; //0x804cc464

in_game bool PlayerLoader_IsLoaded(int* _gPlayerLoader, Players Player); //0x80060be0
in_game void PlayerLoader_PreLoad(int* _gPlayerLoader, Players Player); //0x80060ba4
in_game void PlayerLoader_CancelPreLoad(int* _gPlayerLoader); //0x80060cac
in_game void PlayerLoader_DeLoad(int* _gPlayerLoader, Players Player); //0x80060c74

in_game int gSEFileManager; //0x804606f8

in_game uint* GetPlayersFileList(Players player); //0x80060b18
in_game bool FileManager_IsGeoLoaded(int* _gSEFileManager, uint* pFileHash); //0x8022db20

in_game int Player_Delete(int* self); //0x80058278

in_game void Sparx_SetHealthState(int* self); //0x800f6540
in_game void Sparx_SetMode(int* self, Modes_Sparx mode, int force); //0x800eeff4
in_game void Sparx_HandleAnims(int* self, int mode); //0x800f96c8
in_game void Sparx_HandleHiding(int* self, int modeflags); //0x800f43c0

in_game bool Spyro_TestBreathChange(int* self); //0x8008ae04

in_game void PlayerState_SetHealth(int* self, int health); //0x800689c0
in_game void PlayerState_RestartGame(int* self); //0x800687ec

in_game int* PlayerSetupInfo_GetMap(PlayerSetupInfo* self); //0x80068d8c

in_game EXVector ray0; //0x80449ED0
in_game EXVector ray1; //0x80449EE0
in_game EXVector ray2; //0x80449EF0

//Create a player according to the current PlayerSetup, returns the new object.
in_game int* CreatePlayer(int* self); //0x8005b858
in_game int* Player_SetMode(int* self, int mode, int force, int params); //0x80058414
in_game int* Player_SetVisibility(int* self, bool onoff); //0x80060844
in_game int* Player_TurnOn(int* self, bool onoff, int mode); //0x8005cb40
in_game int* Player_UnlockControls(int* self); //0x8005bdfc

in_game void XSEItemEnv_AddXSEItem(int* _theItemEnv, int* pItem, uint GroupFlags); //0x802409d0
in_game int* XSEItem_CreateObject(void); //0x8023e070

in_game bool ItemHandler_SEUpdate(int* self); //0x80241d64
in_game void ItemHandler_ChangeAnimSkin(int* self, int* animator, uint skinHash); //0x800b9aa8
//Get the handler from the item environment that uses the given handler ID, starting from the given index.
in_game int* ItemEnv_FindUniqueIDHandler(int* self, uint handlerID, int index); //0x80240c48

in_game bool SetCamera(CamTypes type, CamCreateMode mode, int* target, Players targetType, uint SetupFlags); //0x8012a104

in_game int* BaseDisplay_m_pDisplay; //0x804cba84
in_game void SE_Loop_ChildListDraw(int* self, int* pWnd); //0x80230098
in_game void XSprite2D_Draw(XSprite2D* self, int* pWnd); //0x80130db4
in_game void EXWnd_SelectSprite2DTexture(int* self, int* pTexture, bool Filter, bool Wrap); //0x8031ceb0
in_game void SEMap_BallGadget_AddPlayer(int* self, EXVector* position, EXVector* rotation); //0x802331bc

#endif //SYMBOLS_H