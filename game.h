#ifndef _GAME_H
#define _GAME_H

#include "chessSystem.h"
#include "map.h"


#define NUM_OF_PLAYRES_PER_GAME 2

typedef struct Game {
    int players_id[NUM_OF_PLAYRES_PER_GAME];
    Winner result;
    int duration;
} Game;

Game* gameCreate(int first_player,int second_player, Winner winner, int play_time);
MapKeyElement gameCopy(MapKeyElement game);
void gameDestroy(MapKeyElement game);

#endif //_GAME_H
