#ifndef _TOURNAMENT_H
#define _TOURNAMENT_H

#include "map.h"

#define TOURNAMENT_DELETED_PLAYER -1
#define TOURNAMENT_NOT_ENDED -1

typedef struct Tournament {
    Map games;
    char *location;
    int winner;
    int max_games_per_player;
    int next_game_id;
    Map players_stats;
} Tournament;

Tournament* tournamentCreate(copyMapKeyElements copyMapKeyElements, freeMapKeyElements freeMapKeyElements, 
                             compareMapKeyElements compareMapKeyElements,char *location,int max_games_per_player);  
Tournament* tournamentCopy(Tournament* tournament);
bool checkAlreadyPlayed(Tournament* tournament, int player1, int player2);
bool checkExceededGames(Tournament* tournament, int player);
void tournamentDestroy(Tournament* tournament);
bool checkLocation(const char* tournament_location);



#endif //_TOURNAMENT_H