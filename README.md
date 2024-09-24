# Spyro: A Hero's Tail Multiplayer
This is a work-in-progress mod for Spyro: A Hero's Tail that adds support for multiple players.

Created using Composer's [code injection utility](https://github.com/C0mposer/C-Game-Modding-Utility).

<i>Note: This mod is not terribly stable, and crashes/freezes can sometimes occour. While playing, make sure you back up your savedata and/or make regular savestates to fall back on. If Dolphin pops up with an error message like "Invalid read from 0x--------, PC = 0x--------", then feel free to share this information with me so I can track down the bug. You can hit me up on Twitter (@EbbeAHT) or Discord (@ebbeaht).</i>

## Compatibility
This mod is currently limited specifically to the NTSC GameCube version (G5SE7D).

| Platform | NTSC | PAL   |
| -------- | ---- | ----- |
| GameCube | ✅    | ❌     |
| PS2      | ❌    | ❌     |
| Xbox     | ❌    | ❌     |

## Installation
<i>Patches are not available yet, sorry!</i>

A few methods of installation are provided, all of which use [xdelta](https://www.romhacking.net/utilities/598/) for patching the game.

<i>If using Netplay, make sure all players have the same version of the mod installed before playing, or desyncs will occour!</i><br>
<i>If Dolphin is preventing you from starting Netplay or there are frequent desyncs, it might be a good idea to have all players use the same patching method.</i>

### ISO Patch

This patching method is the easiest and might be possible if you have a .iso file of the NTSC GameCube version of the game that Dolphin classifies as a "Good dump" (right click on game->Properties->Verify).

Use `iso_patch.xdelta` to patch the .iso file. If the process fails, your file likely doesn't exactly match the version I used to mod the game, and another method must be used. If you're applying any mods to the game's assets, do so <i>after</i> patching.

### Executable Patch (GCRebuilder)

The tool [GCRebuilder](https://gamebanana.com/tools/6410) can be used to extract and then overwrite the executable after patching, creating a new ISO. <i>Note that this will overwrite the .iso file, so make sure you back up the unmodded .iso beforehand!</i>

Open the .iso in GCRebuilder with image->open, in the "Structure" panel navigate to `&&SystemData` and export `Start.dol`. Apply the patch `startdol_patch.xdelta` to the file. In GCRebuilder, navigate to `Start.dol` again and import the newly patched file overtop of it.

### Executable Patch (Unpack and play from folder)

If this method is used over Netplay, it must be used by all players. Right click on the game in Dolphin, go to Properties, go to the Filesystem tab, right click on the disc and select "Extract Entire Disc". Apply the patch `maindol_patch.xdelta` to the file `sys\main.dol`, and replace that file with the newly patched version.

To play the game, either drag `main.dol` into Dolphin's window, or add the unpacked folder to Dolphin's paths.

## Usage
Any time the game boots up or reloads, only player 1 spawns. Make sure to always have player 1 assigned to a controller.<br>
When a player joins, the game will display names/health/powerup status above them.<br>
Notifications will be shown on the left of the screen when a player joins/leaves/dies/switches breath/is under cooldown.<br>
When the game is paused, a separate menu lets you edit the multiplayer settings.

### Multiplayer Menu
* The "Death Mode" determines what happens when a player dies. "Despawn Player" just makes the player despawn with a cooldown, and "Reload Game" makes the game reload, like in singleplayer. In any case, the game will reload when all players are dead.
* When the Death Mode is set to "Despawn Player", the rejoin cooldown can be adjusted. A cooldown of 0 seconds just lets a player respawn the instant they die. The default is 10 seconds.

### Controls
* Additional players can join in by pressing A.
* Players can leave by holding down dpad-right. If all players leave, the game reloads.
* If a player is locked/invisible, they can hold dpad-up to restore control/visibility. Yes, even during cutscenes. Use with caution.
* Player 1 can hold dpad-left to reload the game.
* Debug information can be toggled when player 1 holds dpad-down.

### Dolphin Netplay
The Netplay feature in the Dolphin Emulator can be used to play with people online, as long as they have the same version of the mod installed.
For details on how to set it up or additional troubleshooting, see [Dolphin's official guide](https://dolphin-emu.org/docs/guides/netplay-guide/).

Netplay will only detect games based on ID, so if you have both the unmodded and modded version of the game in the list, it may pick the wrong one. Set up your paths to only include the modded version of the game to fix this.

### Dolphin Settings
[This Google Doc](https://docs.google.com/document/d/17H4Iix3DcQj252ll0D2zSmDeVXM7V8XGsjBwkQ52ZYQ/edit?usp=sharing) contains a guide on the ideal settings in Dolphin to use with this game.<br>
Multiple players can take quite a toll on the game, so increasing the CPU clockspeed by 50% or so should alleviate some of the performance issues. Make sure this setting is synched across all players.

## Known Issues

* The camera gets stuck in weird ways when trying to frame all players at once, and it doesn't even succeed in that all the time. If an area is giving you trouble, try sticking together or have less players present.
* Cutscenes only target one player when locking controls/making invisible, making it possible for them to lock one player and try unlocking another when it's over. The unlock feature when holding dpad-up can be used to circumvent this.
* The Ball Gadget station can break and lock up if players move around during the transform sequence. Make sure only one player stands on the gadget pad at a time.
* Only player 1 can interact with menus, even if others activate them.
* Attack hitboxes can only target one player at a time, leading to weird situations where attacks phase right through a player. This is especially noticable in bosses.
* While the turret minigames <i>technically</i> work, all players control the same reticle and their firerates overlap.
* Sparx minigames are completely broken and multiplayer for him has been disabled for the moment.
* Switching between Spyro/Hunter is very broken when more than one player is present. Consider leaving just one player to do it.
* When Blink digs through tunnes he leaves all other players behind in the previous room. Make those players either enter the same tunnel or leave and rejoin.
* The ball gadget sections are awful in multiplayer due to the high speeds, frequent bugs and annoying camera. Consider skipping them, letting one player go through them or just embracing the chaos.
* Sparx attempts to reflect the player with the lowest health, but when he's gone he doesn't return after the player dies, even if the remaining player(s) should have Sparx. Eating fodder resolves this.

Note that some issues have been left in intentionally for now, because they are hard to fix or they don't ruin the game, and may actually provide some fun behavior despite not being made "properly". An example would be letting players move around during cutscenes, which can break things a bit, but is generally just fun to mess around with.