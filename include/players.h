#ifndef PLAYERS_H
#define PLAYERS_H
#include <main.h>

extern XRGBA* PLAYER_COLORS[];

//Colors of health when the hitpoint upgrade isn't purchased
extern XRGBA* HEALTH_COLORS_NO_UPGRADE[];

//Colors of health when the hitpoint upgrade is purchased
extern XRGBA* HEALTH_COLORS_UPGRADE[];

//Hotswap values for temporary raycast memory
extern RayVecs PLAYER_RAYVECS[4];

//The names that are displayed above the players
extern char* PLAYER_NAMES[];

//Hotswap values for selected breath
extern Breaths PLAYER_BREATHS[];

//Hotswap values for health
extern int PLAYER_HEALTH[];

//Hotswap values for whether supercharge is active
extern bool PLAYER_SUPERCHARGE[];
//Hotswap values for supercharge timers
extern float PLAYER_SUPERCHARGE_TIMER[];

//Hotswap values for whether invincibility is active
extern bool PLAYER_INVINCIBILITY[];
//Hotswap values for invincibility timers
extern float PLAYER_INVINCIBILITY_TIMER[];

//Contains the itemhandler ID's for each of the four players. -1 if not in use.
extern int players[4];

//Get the number of player items currently kept track of
int NumberOfPlayers();

//Get the number of player items currently kept track of, who can have Sparx following around
int NumberOfPlayersWhoCanHaveSparx();

//Check if the only player is player 1
bool OnlyPlayer1Exists();

//Check if the given handler is the only player left.
bool handlerIsOnlyPlayerLeft(int* handler);

//Get the associated controller port index for the given player handler
int GetPortNrFromPlayerHandler(int* handler);

//Set the global port number, handler reference and handler item reference to the player at the given port number.
//Returns whether the player handler could be found.
bool SetPlayerRefToPort(int portNr);

//Check if the given handler's vtable matches that of a player
bool HandlerIsPlayer(int* handler);

//Runs through all players, inserts the handlers into the given list of size 4, and returns amount of handlers found.
int GetArrayOfPlayerHandlers(int** list);

//Get the first player who has sparx.
int* GetFirstPlayerNotZeroHP();

//Reset all player references and search the item list to populate it again.
void initializePlayers();

//Remove any handler references that don't exist anymore.
//If no players are referenced, do a full initialize of the player list.
void updatePlayerList();

bool modeIsDying(PlayerModes mode);

//Add new player and assign it to the given port number
void addNewPlayer(int portNr);

//Remove a player from the game, restore follower camera, reload game if this was the last player left
void removePlayer(int portNr, bool died);

//Set a player to be visible and controllable
void restorePlayerControl(int portNr);

//Teleport all players to the player described by portNr
void teleportPlayersToPlayer(int portNr);

//Returns whatever pad number is moving the right stick the most
int whoShouldControlCamera();

//Get the middle point between all players, as well as the biggest range between them.
//Returns false if no players were found.
//Tries ignoring any player in the dying state.
bool GetPlayerPosMidAndRanges(EXVector3* middle, float* biggestRange);

//Sets all players' health and the global health to full
void SetAllHealthFull();

#endif