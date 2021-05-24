#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "game.h"



Game* gameCreate(int first_player,int second_player, Winner winner, int play_time)
{
    Game *game=malloc(sizeof(*game));
    if(game == NULL){
        return NULL;
    }
    game->players_id[0]=first_player;
    game->players_id[1]=second_player;
    game->result=winner;
    game->duration=play_time;
    return game;
}

MapKeyElement gameCopy(MapKeyElement game)
{
    Game *game_to_copy = (Game*)game;
    Game* game_copy=gameCreate(game_to_copy->players_id[0],
                                game_to_copy->players_id[1],
                                game_to_copy->result,
                                game_to_copy->duration);
    return (MapKeyElement)game_copy;
}
void gameDestroy(MapKeyElement game)
{
    free(game);
}

