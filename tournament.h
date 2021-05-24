#ifndef _TOURNAMENT_H
#define _TOURNAMENT_H

#include "map.h"

#define TOURNAMENT_DELETED_PLAYER -1
#define TOURNAMENT_NOT_ENDED -1
#define WINS 0
#define LOSSES 1
#define DRAWS 2
#define TIME_PLAYED 3
#define SCORE 4

typedef struct Tournament {
    Map games;
    char *location;
    int winner;
    int max_games_per_player;
    int next_game_id;
    Map players_stats;
} Tournament;

Tournament* tournamentCreate(copyMapKeyElements copyMapKeyElements, freeMapKeyElements freeMapKeyElements, 
                             compareMapKeyElements compareMapKeyElements,const char *location,int max_games_per_player);  
MapDataElement tournamentCopy(MapDataElement tournament);
bool checkAlreadyPlayed(const Tournament* tournament, int player1, int player2);
bool checkExceededGames(const Tournament* tournament, int player);
void tournamentDestroy(MapDataElement tournament);
bool checkLocation(const char* tournament_location);
MapResult updateStats(Map players_stats, int player_id, int wins, int losses, int draws, int time_played);
void tournamentEnd(Tournament* tournament);
bool tournamentRemovePlayer(Tournament* tournament,int player_id);



#endif //_TOURNAMENT_H