#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "game.h"
#include "map.h"
#include "tournament.h"

#define CAPITAL_A "A"
#define CAPITAL_Z "Z"
#define SMALL_A "a"
#define SMALL_Z "z"
#define SPACE " "

int *statsArrCopy(int *stats)

Tournament* tournamentCreate(copyMapKeyElements copyMapKeyElements,freeMapKeyElements freeMapKeyElements, 
                             compareMapKeyElements compareMapKeyElements,char *location,int max_games_per_player)
{
    assert(location != NULL);
    Tournament* tournament = malloc(sizeof(*tournament));
    if (tournament == NULL) {
        return NULL;
    }
    tournament->games = mapCreate(gameCopy,copyMapKeyElements,
                                gameDestroy, freeMapKeyElements,
                                compareMapKeyElements);
    if (tournament->games == NULL) {
        free(tournament);
        return NULL;
    }
    tournament->players_stats = mapCreate(gameCopy,copyMapKeyElements,
                                gameDestroy, freeMapKeyElements,
                                compareMapKeyElements);
    tournament->location = malloc(sizeof(char) * strlen(location) + 1);
    if (tournament->location == NULL) {
        mapDestroy(tournament->games);
        free(tournament);
        return NULL;
    }
    strcpy(tournament->location, location);
    tournament->winner = TOURNAMENT_NOT_ENDED;
    tournament->max_games_per_player=max_games_per_player;
    tournament->next_game_id = 0;
    return tournament;
}
static Tournament* tournamentCopyData(Map games, char *location, int winner,
                                    int max_games_per_player,int next_game_id)
{   
    assert(games != NULL);
    assert(location != NULL);

    Tournament* tournament = malloc(sizeof(*tournament));
    if (tournament == NULL) {
        return NULL;
    }
    tournament->games = mapCopy(games);
    if (tournament->games == NULL) {
        free(tournament);
        return NULL;
    }
    tournament->location = malloc(sizeof(char) * strlen(location) + 1);
    if (tournament->location == NULL) {
        mapDestroy(tournament->games);
        free(tournament);
        return NULL;
    }
    strcpy(tournament->location, location);
    tournament->winner = winner;
    tournament->next_game_id = next_game_id;
    tournament->max_games_per_player = max_games_per_player;
    return tournament;
}

Tournament* tournamentCopy(Tournament* tournament)
{
    if (tournament == NULL) {
        return NULL;
    }
    Tournament* tournament_copy = tournamentCopyData(tournament->games,
                                                    tournament->location,
                                                    tournament->winner,
                                                    tournament->max_games_per_player,
                                                    tournament->next_game_id);
   
    return tournament_copy;
}

bool checkAlreadyPlayed(Tournament* tournament, int player1, int player2)
{
    assert(tournament != NULL);
    MAP_FOREACH(int *, iter, tournament->games) {
        Game* game = (Game*)mapGet(tournament->games, iter);
        if (game->players_id[0] == player1 && game->players_id[1] == player2) {
            return true;
        }
        if (game->players_id[0] == player2 && game->players_id[1] == player1) {
            return true;
        }
    }
    return false;
}

bool checkExceededGames(Tournament* tournament, int player)
{
    assert(tournament != NULL);
    int count_games = 0;
    MAP_FOREACH(int *, iter, tournament->games) {
         Game* game = (Game*)mapGet(tournament->games, iter);
         if (game->players_id[0] == player || game->players_id[1] == player) {
             count_games++;
         }
    }
    return count_games >= tournament->max_games_per_player;
}

void tournamentDestroy(Tournament* tournament)
{
    if (tournament != NULL) {
        free(tournament->location);
        mapDestroy(tournament->games);
        free(tournament);
    }    
}

bool checkLocation(const char* tournament_location)
{
    if (tournament_location == NULL || strlen(tournament_location) == 0) {
        return false;
    }

    if (tournament_location  < CAPITAL_A || tournament_location > CAPITAL_Z) {
        return false;
    }
    tournament_location++;
    while (tournament_location) {
        if (tournament_location == SPACE || (tournament_location >= SMALL_A && tournament_location <= SMALL_Z)) {
            tournament_location++;
        }
        else {
            return false;
        }
    }
    return true;
}
bool tournamentRemovePlayer(Tournament* tournament,int player_id)
{
    assert(tournament != NULL);
    MAP_FOREACH(int *, iter, tournament->games) {
         Game* game = (Game*)mapGet(tournament->games, iter);
         if (game->players_id[0] == player_id){
             game->result=game->players_id[1];
             game->players_id[0]= TOURNAMENT_DELETED_PLAYER;
             return true;
         }
         if (game->players_id[1] == player_id){
             game->result=game->players_id[0];
             game->players_id[1]= TOURNAMENT_DELETED_PLAYER;
             return true;
         }
    }
    return false;
}

int computeMaxRank(Tournament* tournament)
{
    assert(tournament != NULL);
    
}
