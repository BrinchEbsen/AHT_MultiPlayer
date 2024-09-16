INPUT(../../../.config/symbols/symbols.txt)
INPUT(../../../.config/symbols/function_symbols.txt)
INPUT(../../../.config/symbols/auto_symbols.txt)

MEMORY
{
    /* RAM locations where we'll inject the code for our replacement functions */
    MainCodecave : ORIGIN = 0x80296618, LENGTH = 0x7DCC
    patchFPMode1 : ORIGIN = 0x8005ca40, LENGTH = 0x100000
    patchFPMode2 : ORIGIN = 0x8005c9fc, LENGTH = 0x100000
    patchHandleCharge : ORIGIN = 0x800a16ac, LENGTH = 0x100000
    patchFileManager : ORIGIN = 0x8022d150, LENGTH = 0x100000
    patchCamAutoTurn : ORIGIN = 0x8011d670, LENGTH = 0x100000
    patchCamCollisionUpdate : ORIGIN = 0x8012162c, LENGTH = 0x100000
    hookItemHandlerUpdate : ORIGIN = 0x8023dfec, LENGTH = 0x100000
    hookRetrace : ORIGIN = 0x8026fa6c, LENGTH = 0x100000
    hookScanMode : ORIGIN = 0x8005c954, LENGTH = 0x100000
    hookHUDDraw : ORIGIN = 0x80222244, LENGTH = 0x100000
    patchHandleJumpCharge : ORIGIN = 0x800aad94, LENGTH = 0x100000
    patchHandlePoleGrab1 : ORIGIN = 0x800b6870, LENGTH = 0x100000
    patchHandlePoleGrab2 : ORIGIN = 0x800b6710, LENGTH = 0x100000
    patchHandlePoleGrab3 : ORIGIN = 0x800b7fe4, LENGTH = 0x100000
    patchFireCannon1 : ORIGIN = 0x800ae7a4, LENGTH = 0x100000
    patchFireCannon2 : ORIGIN = 0x800ae8bc, LENGTH = 0x100000
    patchHandleWaterDive1 : ORIGIN = 0x8008f458, LENGTH = 0x100000
    patchHandleWaterDive2 : ORIGIN = 0x8008f4c8, LENGTH = 0x100000
    patchHandleWaterDive3 : ORIGIN = 0x8008f64c, LENGTH = 0x100000
    patchHandleWaterDive4 : ORIGIN = 0x8008f580, LENGTH = 0x100000
    patchHandleWaterDive5 : ORIGIN = 0x8008f880, LENGTH = 0x100000
    hookTestBreathChange : ORIGIN = 0x8008707c, LENGTH = 0x100000
    hookSparxSetPlayerHealth1 : ORIGIN = 0x800f2944, LENGTH = 0x100000
    hookSparxSetPlayerHealth2 : ORIGIN = 0x800f2b20, LENGTH = 0x100000
    hookSparxSetPlayerHealth3 : ORIGIN = 0x8015dee8, LENGTH = 0x100000
    patchHandleWallClimb1 : ORIGIN = 0x80056174, LENGTH = 0x100000
    patchHandleWallClimb2 : ORIGIN = 0x80056240, LENGTH = 0x100000
    patchHandleFireBow1 : ORIGIN = 0x8004f968, LENGTH = 0x100000
    patchHandleFireBow2 : ORIGIN = 0x8004f9a0, LENGTH = 0x100000
    hookSCPad : ORIGIN = 0x800e9ad0, LENGTH = 0x100000
    hookINVPad : ORIGIN = 0x800e9b18, LENGTH = 0x100000
    patchHandleSuperCharge1 : ORIGIN = 0x800a1e4c, LENGTH = 0x100000
    patchHandleSuperCharge2 : ORIGIN = 0x800a1d6c, LENGTH = 0x100000
    hookReImpl_PlayerDelete : ORIGIN = 0x80058278, LENGTH = 0x100000
    hookSparxDoKill_Spy : ORIGIN = 0x80086988, LENGTH = 0x100000
    hookSparxSetNull_Spy : ORIGIN = 0x80086990, LENGTH = 0x100000
    hookSparxDoKill_Blk : ORIGIN = 0x8001ff28, LENGTH = 0x100000
    hookSparxSetNull_Blk : ORIGIN = 0x8001ff30, LENGTH = 0x100000
    hookSparxDoKill_Hun : ORIGIN = 0x80046dcc, LENGTH = 0x100000
    hookSparxSetNull_Hun : ORIGIN = 0x80046dd4, LENGTH = 0x100000
    hookSparxDoKill_Sgt : ORIGIN = 0x8007b590, LENGTH = 0x100000
    hookSparxSetNull_Sgt : ORIGIN = 0x8007b598, LENGTH = 0x100000
    hookUpdateItemPhysics : ORIGIN = 0x8023f7f4, LENGTH = 0x100000
    hookReImpl_SpyroDeath : ORIGIN = 0x800a31b0, LENGTH = 0x100000
    hookReImpl_BlinkDeath : ORIGIN = 0x80020ebc, LENGTH = 0x100000
    hookReImpl_HunterDeath : ORIGIN = 0x80047f4c, LENGTH = 0x100000
    hookReImpl_ByrdDeath : ORIGIN = 0x8007c440, LENGTH = 0x100000
    hookChangePlayer : ORIGIN = 0x8005bd04, LENGTH = 0x100000
    patchHunterSwampDeath : ORIGIN = 0x8004f0d0, LENGTH = 0x100000
    hookHunterDeathFall : ORIGIN = 0x80047a5c, LENGTH = 0x100000
    hookSpyroDeathFall : ORIGIN = 0x8008a25c, LENGTH = 0x100000
    patchSpyroSwampDeath : ORIGIN = 0x800a3490, LENGTH = 0x100000
    patchCallPlayerWndDraw : ORIGIN = 0x80131e28, LENGTH = 0x100000
    patchBlinkSwampDeath : ORIGIN = 0x8002e290, LENGTH = 0x100000
    patchSgtHandleFall1 : ORIGIN = 0x800819ac, LENGTH = 0x100000
    patchSgtHandleFall2 : ORIGIN = 0x80081a9c, LENGTH = 0x100000
    patchSgtHandleCrashWater : ORIGIN = 0x800838f8, LENGTH = 0x100000
    patchSgtHandleCrashLava : ORIGIN = 0x80083b04, LENGTH = 0x100000
    hookSgtDeathFall : ORIGIN = 0x8007c07c, LENGTH = 0x100000
    patchCamBGCollisionUpdate : ORIGIN = 0x8010f248, LENGTH = 0x100000
}

SECTIONS
{
    /* Custom section for compiled code */
    /* Custom section for our hook code */
    .patchFPMode1 : 
    {
        patchFPMode1.o(.text)
        patchFPMode1.o(.rodata)
        patchFPMode1.o(.rodata*)
        patchFPMode1.o(.data)
        patchFPMode1.o(.bss)
    } > patchFPMode1

    /* Custom section for our hook code */
    .patchFPMode2 : 
    {
        patchFPMode2.o(.text)
        patchFPMode2.o(.rodata)
        patchFPMode2.o(.rodata*)
        patchFPMode2.o(.data)
        patchFPMode2.o(.bss)
    } > patchFPMode2

    /* Custom section for our hook code */
    .patchHandleCharge : 
    {
        patchHandleCharge.o(.text)
        patchHandleCharge.o(.rodata)
        patchHandleCharge.o(.rodata*)
        patchHandleCharge.o(.data)
        patchHandleCharge.o(.bss)
    } > patchHandleCharge

    /* Custom section for our hook code */
    .patchFileManager : 
    {
        file_manager_patch.o(.text)
        file_manager_patch.o(.rodata)
        file_manager_patch.o(.rodata*)
        file_manager_patch.o(.data)
        file_manager_patch.o(.bss)
    } > patchFileManager

    /* Custom section for our hook code */
    .patchCamAutoTurn : 
    {
        cam_auto_turn.o(.text)
        cam_auto_turn.o(.rodata)
        cam_auto_turn.o(.rodata*)
        cam_auto_turn.o(.data)
        cam_auto_turn.o(.bss)
    } > patchCamAutoTurn

    /* Custom section for our hook code */
    .patchCamCollisionUpdate : 
    {
        cam_coll_update.o(.text)
        cam_coll_update.o(.rodata)
        cam_coll_update.o(.rodata*)
        cam_coll_update.o(.data)
        cam_coll_update.o(.bss)
    } > patchCamCollisionUpdate

    /* Custom section for our hook code */
    .hookItemHandlerUpdate : 
    {
        itemhandler_hook.o(.text)
        itemhandler_hook.o(.rodata)
        itemhandler_hook.o(.rodata*)
        itemhandler_hook.o(.data)
        itemhandler_hook.o(.bss)
    } > hookItemHandlerUpdate

    /* Custom section for our hook code */
    .hookRetrace : 
    {
        main_hook.o(.text)
        main_hook.o(.rodata)
        main_hook.o(.rodata*)
        main_hook.o(.data)
        main_hook.o(.bss)
    } > hookRetrace

    /* Custom section for our hook code */
    .hookScanMode : 
    {
        scan_hook.o(.text)
        scan_hook.o(.rodata)
        scan_hook.o(.rodata*)
        scan_hook.o(.data)
        scan_hook.o(.bss)
    } > hookScanMode

    /* Custom section for our hook code */
    .hookHUDDraw : 
    {
        draw_hook.o(.text)
        draw_hook.o(.rodata)
        draw_hook.o(.rodata*)
        draw_hook.o(.data)
        draw_hook.o(.bss)
    } > hookHUDDraw

    /* Custom section for our hook code */
    .patchHandleJumpCharge : 
    {
        patchHandleJumpCharge.o(.text)
        patchHandleJumpCharge.o(.rodata)
        patchHandleJumpCharge.o(.rodata*)
        patchHandleJumpCharge.o(.data)
        patchHandleJumpCharge.o(.bss)
    } > patchHandleJumpCharge

    /* Custom section for our hook code */
    .patchHandlePoleGrab1 : 
    {
        patchHandlePoleGrab1.o(.text)
        patchHandlePoleGrab1.o(.rodata)
        patchHandlePoleGrab1.o(.rodata*)
        patchHandlePoleGrab1.o(.data)
        patchHandlePoleGrab1.o(.bss)
    } > patchHandlePoleGrab1

    /* Custom section for our hook code */
    .patchHandlePoleGrab2 : 
    {
        patchHandlePoleGrab2.o(.text)
        patchHandlePoleGrab2.o(.rodata)
        patchHandlePoleGrab2.o(.rodata*)
        patchHandlePoleGrab2.o(.data)
        patchHandlePoleGrab2.o(.bss)
    } > patchHandlePoleGrab2

    /* Custom section for our hook code */
    .patchHandlePoleGrab3 : 
    {
        patchHandlePoleGrab3.o(.text)
        patchHandlePoleGrab3.o(.rodata)
        patchHandlePoleGrab3.o(.rodata*)
        patchHandlePoleGrab3.o(.data)
        patchHandlePoleGrab3.o(.bss)
    } > patchHandlePoleGrab3

    /* Custom section for our hook code */
    .patchFireCannon1 : 
    {
        patchFireCannon1.o(.text)
        patchFireCannon1.o(.rodata)
        patchFireCannon1.o(.rodata*)
        patchFireCannon1.o(.data)
        patchFireCannon1.o(.bss)
    } > patchFireCannon1

    /* Custom section for our hook code */
    .patchFireCannon2 : 
    {
        patchFireCannon2.o(.text)
        patchFireCannon2.o(.rodata)
        patchFireCannon2.o(.rodata*)
        patchFireCannon2.o(.data)
        patchFireCannon2.o(.bss)
    } > patchFireCannon2

    /* Custom section for our hook code */
    .patchHandleWaterDive1 : 
    {
        patchHandleWaterDive1.o(.text)
        patchHandleWaterDive1.o(.rodata)
        patchHandleWaterDive1.o(.rodata*)
        patchHandleWaterDive1.o(.data)
        patchHandleWaterDive1.o(.bss)
    } > patchHandleWaterDive1

    /* Custom section for our hook code */
    .patchHandleWaterDive2 : 
    {
        patchHandleWaterDive2.o(.text)
        patchHandleWaterDive2.o(.rodata)
        patchHandleWaterDive2.o(.rodata*)
        patchHandleWaterDive2.o(.data)
        patchHandleWaterDive2.o(.bss)
    } > patchHandleWaterDive2

    /* Custom section for our hook code */
    .patchHandleWaterDive3 : 
    {
        patchHandleWaterDive3.o(.text)
        patchHandleWaterDive3.o(.rodata)
        patchHandleWaterDive3.o(.rodata*)
        patchHandleWaterDive3.o(.data)
        patchHandleWaterDive3.o(.bss)
    } > patchHandleWaterDive3

    /* Custom section for our hook code */
    .patchHandleWaterDive4 : 
    {
        patchHandleWaterDive4.o(.text)
        patchHandleWaterDive4.o(.rodata)
        patchHandleWaterDive4.o(.rodata*)
        patchHandleWaterDive4.o(.data)
        patchHandleWaterDive4.o(.bss)
    } > patchHandleWaterDive4

    /* Custom section for our hook code */
    .patchHandleWaterDive5 : 
    {
        patchHandleWaterDive5.o(.text)
        patchHandleWaterDive5.o(.rodata)
        patchHandleWaterDive5.o(.rodata*)
        patchHandleWaterDive5.o(.data)
        patchHandleWaterDive5.o(.bss)
    } > patchHandleWaterDive5

    /* Custom section for our hook code */
    .hookTestBreathChange : 
    {
        hookTestBreathChange.o(.text)
        hookTestBreathChange.o(.rodata)
        hookTestBreathChange.o(.rodata*)
        hookTestBreathChange.o(.data)
        hookTestBreathChange.o(.bss)
    } > hookTestBreathChange

    /* Custom section for our hook code */
    .hookSparxSetPlayerHealth1 : 
    {
        hookSparxSetPlayerHealth1.o(.text)
        hookSparxSetPlayerHealth1.o(.rodata)
        hookSparxSetPlayerHealth1.o(.rodata*)
        hookSparxSetPlayerHealth1.o(.data)
        hookSparxSetPlayerHealth1.o(.bss)
    } > hookSparxSetPlayerHealth1

    /* Custom section for our hook code */
    .hookSparxSetPlayerHealth2 : 
    {
        hookSparxSetPlayerHealth2.o(.text)
        hookSparxSetPlayerHealth2.o(.rodata)
        hookSparxSetPlayerHealth2.o(.rodata*)
        hookSparxSetPlayerHealth2.o(.data)
        hookSparxSetPlayerHealth2.o(.bss)
    } > hookSparxSetPlayerHealth2

    /* Custom section for our hook code */
    .hookSparxSetPlayerHealth3 : 
    {
        hookSparxSetPlayerHealth3.o(.text)
        hookSparxSetPlayerHealth3.o(.rodata)
        hookSparxSetPlayerHealth3.o(.rodata*)
        hookSparxSetPlayerHealth3.o(.data)
        hookSparxSetPlayerHealth3.o(.bss)
    } > hookSparxSetPlayerHealth3

    /* Custom section for our hook code */
    .patchHandleWallClimb1 : 
    {
        patchHandleWallClimb1.o(.text)
        patchHandleWallClimb1.o(.rodata)
        patchHandleWallClimb1.o(.rodata*)
        patchHandleWallClimb1.o(.data)
        patchHandleWallClimb1.o(.bss)
    } > patchHandleWallClimb1

    /* Custom section for our hook code */
    .patchHandleWallClimb2 : 
    {
        patchHandleWallClimb2.o(.text)
        patchHandleWallClimb2.o(.rodata)
        patchHandleWallClimb2.o(.rodata*)
        patchHandleWallClimb2.o(.data)
        patchHandleWallClimb2.o(.bss)
    } > patchHandleWallClimb2

    /* Custom section for our hook code */
    .patchHandleFireBow1 : 
    {
        patchHandleFireBow1.o(.text)
        patchHandleFireBow1.o(.rodata)
        patchHandleFireBow1.o(.rodata*)
        patchHandleFireBow1.o(.data)
        patchHandleFireBow1.o(.bss)
    } > patchHandleFireBow1

    /* Custom section for our hook code */
    .patchHandleFireBow2 : 
    {
        patchHandleFireBow2.o(.text)
        patchHandleFireBow2.o(.rodata)
        patchHandleFireBow2.o(.rodata*)
        patchHandleFireBow2.o(.data)
        patchHandleFireBow2.o(.bss)
    } > patchHandleFireBow2

    /* Custom section for our hook code */
    .hookSCPad : 
    {
        hookSCPad.o(.text)
        hookSCPad.o(.rodata)
        hookSCPad.o(.rodata*)
        hookSCPad.o(.data)
        hookSCPad.o(.bss)
    } > hookSCPad

    /* Custom section for our hook code */
    .hookINVPad : 
    {
        hookINVPad.o(.text)
        hookINVPad.o(.rodata)
        hookINVPad.o(.rodata*)
        hookINVPad.o(.data)
        hookINVPad.o(.bss)
    } > hookINVPad

    /* Custom section for our hook code */
    .patchHandleSuperCharge1 : 
    {
        patchHandleSuperCharge1.o(.text)
        patchHandleSuperCharge1.o(.rodata)
        patchHandleSuperCharge1.o(.rodata*)
        patchHandleSuperCharge1.o(.data)
        patchHandleSuperCharge1.o(.bss)
    } > patchHandleSuperCharge1

    /* Custom section for our hook code */
    .patchHandleSuperCharge2 : 
    {
        patchHandleSuperCharge2.o(.text)
        patchHandleSuperCharge2.o(.rodata)
        patchHandleSuperCharge2.o(.rodata*)
        patchHandleSuperCharge2.o(.data)
        patchHandleSuperCharge2.o(.bss)
    } > patchHandleSuperCharge2

    /* Custom section for our hook code */
    .hookReImpl_PlayerDelete : 
    {
        hookReImpl_PlayerDelete.o(.text)
        hookReImpl_PlayerDelete.o(.rodata)
        hookReImpl_PlayerDelete.o(.rodata*)
        hookReImpl_PlayerDelete.o(.data)
        hookReImpl_PlayerDelete.o(.bss)
    } > hookReImpl_PlayerDelete

    /* Custom section for our hook code */
    .hookSparxDoKill_Spy : 
    {
        hookSparxDoKill_Spy.o(.text)
        hookSparxDoKill_Spy.o(.rodata)
        hookSparxDoKill_Spy.o(.rodata*)
        hookSparxDoKill_Spy.o(.data)
        hookSparxDoKill_Spy.o(.bss)
    } > hookSparxDoKill_Spy

    /* Custom section for our hook code */
    .hookSparxSetNull_Spy : 
    {
        hookSparxSetNull_Spy.o(.text)
        hookSparxSetNull_Spy.o(.rodata)
        hookSparxSetNull_Spy.o(.rodata*)
        hookSparxSetNull_Spy.o(.data)
        hookSparxSetNull_Spy.o(.bss)
    } > hookSparxSetNull_Spy

    /* Custom section for our hook code */
    .hookSparxDoKill_Blk : 
    {
        hookSparxDoKill_Blk.o(.text)
        hookSparxDoKill_Blk.o(.rodata)
        hookSparxDoKill_Blk.o(.rodata*)
        hookSparxDoKill_Blk.o(.data)
        hookSparxDoKill_Blk.o(.bss)
    } > hookSparxDoKill_Blk

    /* Custom section for our hook code */
    .hookSparxSetNull_Blk : 
    {
        hookSparxSetNull_Blk.o(.text)
        hookSparxSetNull_Blk.o(.rodata)
        hookSparxSetNull_Blk.o(.rodata*)
        hookSparxSetNull_Blk.o(.data)
        hookSparxSetNull_Blk.o(.bss)
    } > hookSparxSetNull_Blk

    /* Custom section for our hook code */
    .hookSparxDoKill_Hun : 
    {
        hookSparxDoKill_Hun.o(.text)
        hookSparxDoKill_Hun.o(.rodata)
        hookSparxDoKill_Hun.o(.rodata*)
        hookSparxDoKill_Hun.o(.data)
        hookSparxDoKill_Hun.o(.bss)
    } > hookSparxDoKill_Hun

    /* Custom section for our hook code */
    .hookSparxSetNull_Hun : 
    {
        hookSparxSetNull_Hun.o(.text)
        hookSparxSetNull_Hun.o(.rodata)
        hookSparxSetNull_Hun.o(.rodata*)
        hookSparxSetNull_Hun.o(.data)
        hookSparxSetNull_Hun.o(.bss)
    } > hookSparxSetNull_Hun

    /* Custom section for our hook code */
    .hookSparxDoKill_Sgt : 
    {
        hookSparxDoKill_Sgt.o(.text)
        hookSparxDoKill_Sgt.o(.rodata)
        hookSparxDoKill_Sgt.o(.rodata*)
        hookSparxDoKill_Sgt.o(.data)
        hookSparxDoKill_Sgt.o(.bss)
    } > hookSparxDoKill_Sgt

    /* Custom section for our hook code */
    .hookSparxSetNull_Sgt : 
    {
        hookSparxSetNull_Sgt.o(.text)
        hookSparxSetNull_Sgt.o(.rodata)
        hookSparxSetNull_Sgt.o(.rodata*)
        hookSparxSetNull_Sgt.o(.data)
        hookSparxSetNull_Sgt.o(.bss)
    } > hookSparxSetNull_Sgt

    /* Custom section for our hook code */
    .hookUpdateItemPhysics : 
    {
        hookUpdateItemPhysics.o(.text)
        hookUpdateItemPhysics.o(.rodata)
        hookUpdateItemPhysics.o(.rodata*)
        hookUpdateItemPhysics.o(.data)
        hookUpdateItemPhysics.o(.bss)
    } > hookUpdateItemPhysics

    /* Custom section for our hook code */
    .hookReImpl_SpyroDeath : 
    {
        hookReImpl_SpyroDeath.o(.text)
        hookReImpl_SpyroDeath.o(.rodata)
        hookReImpl_SpyroDeath.o(.rodata*)
        hookReImpl_SpyroDeath.o(.data)
        hookReImpl_SpyroDeath.o(.bss)
    } > hookReImpl_SpyroDeath

    /* Custom section for our hook code */
    .hookReImpl_BlinkDeath : 
    {
        hookReImpl_BlinkDeath.o(.text)
        hookReImpl_BlinkDeath.o(.rodata)
        hookReImpl_BlinkDeath.o(.rodata*)
        hookReImpl_BlinkDeath.o(.data)
        hookReImpl_BlinkDeath.o(.bss)
    } > hookReImpl_BlinkDeath

    /* Custom section for our hook code */
    .hookReImpl_HunterDeath : 
    {
        hookReImpl_HunterDeath.o(.text)
        hookReImpl_HunterDeath.o(.rodata)
        hookReImpl_HunterDeath.o(.rodata*)
        hookReImpl_HunterDeath.o(.data)
        hookReImpl_HunterDeath.o(.bss)
    } > hookReImpl_HunterDeath

    /* Custom section for our hook code */
    .hookReImpl_ByrdDeath : 
    {
        hookReImpl_ByrdDeath.o(.text)
        hookReImpl_ByrdDeath.o(.rodata)
        hookReImpl_ByrdDeath.o(.rodata*)
        hookReImpl_ByrdDeath.o(.data)
        hookReImpl_ByrdDeath.o(.bss)
    } > hookReImpl_ByrdDeath

    /* Custom section for our hook code */
    .hookChangePlayer : 
    {
        hookChangePlayer.o(.text)
        hookChangePlayer.o(.rodata)
        hookChangePlayer.o(.rodata*)
        hookChangePlayer.o(.data)
        hookChangePlayer.o(.bss)
    } > hookChangePlayer

    /* Custom section for our hook code */
    .patchHunterSwampDeath : 
    {
        patchHunterSwampDeath.o(.text)
        patchHunterSwampDeath.o(.rodata)
        patchHunterSwampDeath.o(.rodata*)
        patchHunterSwampDeath.o(.data)
        patchHunterSwampDeath.o(.bss)
    } > patchHunterSwampDeath

    /* Custom section for our hook code */
    .hookHunterDeathFall : 
    {
        hookHunterDeathFall.o(.text)
        hookHunterDeathFall.o(.rodata)
        hookHunterDeathFall.o(.rodata*)
        hookHunterDeathFall.o(.data)
        hookHunterDeathFall.o(.bss)
    } > hookHunterDeathFall

    /* Custom section for our hook code */
    .hookSpyroDeathFall : 
    {
        hookSpyroDeathFall.o(.text)
        hookSpyroDeathFall.o(.rodata)
        hookSpyroDeathFall.o(.rodata*)
        hookSpyroDeathFall.o(.data)
        hookSpyroDeathFall.o(.bss)
    } > hookSpyroDeathFall

    /* Custom section for our hook code */
    .patchSpyroSwampDeath : 
    {
        patchSpyroSwampDeath.o(.text)
        patchSpyroSwampDeath.o(.rodata)
        patchSpyroSwampDeath.o(.rodata*)
        patchSpyroSwampDeath.o(.data)
        patchSpyroSwampDeath.o(.bss)
    } > patchSpyroSwampDeath

    /* Custom section for our hook code */
    .patchCallPlayerWndDraw : 
    {
        patchCallPlayerWndDraw.o(.text)
        patchCallPlayerWndDraw.o(.rodata)
        patchCallPlayerWndDraw.o(.rodata*)
        patchCallPlayerWndDraw.o(.data)
        patchCallPlayerWndDraw.o(.bss)
    } > patchCallPlayerWndDraw

    /* Custom section for our hook code */
    .patchBlinkSwampDeath : 
    {
        patchBlinkSwampDeath.o(.text)
        patchBlinkSwampDeath.o(.rodata)
        patchBlinkSwampDeath.o(.rodata*)
        patchBlinkSwampDeath.o(.data)
        patchBlinkSwampDeath.o(.bss)
    } > patchBlinkSwampDeath

    /* Custom section for our hook code */
    .patchSgtHandleFall1 : 
    {
        patchSgtHandleFall1.o(.text)
        patchSgtHandleFall1.o(.rodata)
        patchSgtHandleFall1.o(.rodata*)
        patchSgtHandleFall1.o(.data)
        patchSgtHandleFall1.o(.bss)
    } > patchSgtHandleFall1

    /* Custom section for our hook code */
    .patchSgtHandleFall2 : 
    {
        patchSgtHandleFall2.o(.text)
        patchSgtHandleFall2.o(.rodata)
        patchSgtHandleFall2.o(.rodata*)
        patchSgtHandleFall2.o(.data)
        patchSgtHandleFall2.o(.bss)
    } > patchSgtHandleFall2

    /* Custom section for our hook code */
    .patchSgtHandleCrashWater : 
    {
        patchSgtHandleCrashWater.o(.text)
        patchSgtHandleCrashWater.o(.rodata)
        patchSgtHandleCrashWater.o(.rodata*)
        patchSgtHandleCrashWater.o(.data)
        patchSgtHandleCrashWater.o(.bss)
    } > patchSgtHandleCrashWater

    /* Custom section for our hook code */
    .patchSgtHandleCrashLava : 
    {
        patchSgtHandleCrashLava.o(.text)
        patchSgtHandleCrashLava.o(.rodata)
        patchSgtHandleCrashLava.o(.rodata*)
        patchSgtHandleCrashLava.o(.data)
        patchSgtHandleCrashLava.o(.bss)
    } > patchSgtHandleCrashLava

    /* Custom section for our hook code */
    .hookSgtDeathFall : 
    {
        hookSgtDeathFall.o(.text)
        hookSgtDeathFall.o(.rodata)
        hookSgtDeathFall.o(.rodata*)
        hookSgtDeathFall.o(.data)
        hookSgtDeathFall.o(.bss)
    } > hookSgtDeathFall

    /* Custom section for our hook code */
    .patchCamBGCollisionUpdate : 
    {
        cam_BG_coll_update.o(.text)
        cam_BG_coll_update.o(.rodata)
        cam_BG_coll_update.o(.rodata*)
        cam_BG_coll_update.o(.data)
        cam_BG_coll_update.o(.bss)
    } > patchCamBGCollisionUpdate

    .MainCodecave : 
    {
        main.o(.text)
        main.o(.rodata)
        main.o(.rodata*)
        main.o(.data)
        main.o(.bss)
        main.o(.sdata)
        main.o(.sbss)
        main.o(.scommon)
        main.o(.main.o.*)
        buttons.o(.text)
        buttons.o(.rodata)
        buttons.o(.rodata*)
        buttons.o(.data)
        buttons.o(.bss)
        buttons.o(.sdata)
        buttons.o(.sbss)
        buttons.o(.scommon)
        buttons.o(.buttons.o.*)
        colors.o(.text)
        colors.o(.rodata)
        colors.o(.rodata*)
        colors.o(.data)
        colors.o(.bss)
        colors.o(.sdata)
        colors.o(.sbss)
        colors.o(.scommon)
        colors.o(.colors.o.*)
        textprint.o(.text)
        textprint.o(.rodata)
        textprint.o(.rodata*)
        textprint.o(.data)
        textprint.o(.bss)
        textprint.o(.sdata)
        textprint.o(.sbss)
        textprint.o(.scommon)
        textprint.o(.textprint.o.*)
        screenmath.o(.text)
        screenmath.o(.rodata)
        screenmath.o(.rodata*)
        screenmath.o(.data)
        screenmath.o(.bss)
        screenmath.o(.sdata)
        screenmath.o(.sbss)
        screenmath.o(.scommon)
        screenmath.o(.screenmath.o.*)
        *(.text) /* Last section, place any potential remaining code sections */
        *(.branch_lt)
    } > MainCodecave

    /DISCARD/ :
    {
        *(.comment)
        *(.pdr)
        *(.mdebug)
        *(.reginfo)
        *(.MIPS.abiflags)
        *(.eh_frame)
        *(.gnu.attributes)
    }
}