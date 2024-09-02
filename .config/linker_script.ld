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
    hookBlinkDead : ORIGIN = 0x80020f88, LENGTH = 0x100000
    hookHunterDead : ORIGIN = 0x80047f84, LENGTH = 0x100000
    hookSpyroDead : ORIGIN = 0x800a31e8, LENGTH = 0x100000
    hookTestBreathChange : ORIGIN = 0x8008707c, LENGTH = 0x100000
    hookSparxSetPlayerHealth1 : ORIGIN = 0x800f2944, LENGTH = 0x100000
    hookSparxSetPlayerHealth2 : ORIGIN = 0x800f2b20, LENGTH = 0x100000
    hookSparxSetPlayerHealth3 : ORIGIN = 0x8015dee8, LENGTH = 0x100000
    patchHandleWallClimb1 : ORIGIN = 0x80056174, LENGTH = 0x100000
    patchHandleWallClimb2 : ORIGIN = 0x80056240, LENGTH = 0x100000
    patchHandleFireBow1 : ORIGIN = 0x8004f968, LENGTH = 0x100000
    patchHandleFireBow2 : ORIGIN = 0x8004f9a0, LENGTH = 0x100000
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
    .hookBlinkDead : 
    {
        hookBlinkDead.o(.text)
        hookBlinkDead.o(.rodata)
        hookBlinkDead.o(.rodata*)
        hookBlinkDead.o(.data)
        hookBlinkDead.o(.bss)
    } > hookBlinkDead

    /* Custom section for our hook code */
    .hookHunterDead : 
    {
        hookHunterDead.o(.text)
        hookHunterDead.o(.rodata)
        hookHunterDead.o(.rodata*)
        hookHunterDead.o(.data)
        hookHunterDead.o(.bss)
    } > hookHunterDead

    /* Custom section for our hook code */
    .hookSpyroDead : 
    {
        hookSpyroDead.o(.text)
        hookSpyroDead.o(.rodata)
        hookSpyroDead.o(.rodata*)
        hookSpyroDead.o(.data)
        hookSpyroDead.o(.bss)
    } > hookSpyroDead

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