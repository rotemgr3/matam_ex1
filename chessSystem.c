#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "map.h"
#include "chessSystem.h"
#include "game.h"
#include "tournament.h"


typedef struct chess_system_t {
    Map tournaments;
} chess_system_t;

ChessSystem chessCreate()
{
    ChessSystem chess = malloc(sizeof(*chess));
    if (chess == NULL) {
        return NULL;
    }
    chess->tournaments = mapCreate(tournamentCopy,
              copyKeyInt,
              tournamentDestroy,
              freeInt,
              compareInts);
    if (chess->tournaments == NULL) {
        free(chess);
        return NULL;
    }
    return chess;
}

void chessDestroy(ChessSystem chess)
{
    if (chess != NULL) {
        mapDestroy(chess->tournaments);
        free(chess);
    }
}

ChessResult chessAddTournament (ChessSystem chess, int tournament_id,
                                int max_games_per_player, const char* tournament_location)
{
    if (chess == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id <= 0) {
        return CHESS_INVALID_ID;
    }
    if(!checkLocation(tournament_location)) {
        return CHESS_INVALID_LOCATION;
    }
    if(mapContains(chess->tournaments, tournament_id)){
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    Tournament *new_tournament=tournamentCreate(copyKeyInt,
                                               freeInt,
                                               compareInts,
                                               tournament_location,
                                               max_games_per_player);
    if(new_tournament==NULL){
        return CHESS_OUT_OF_MEMORY;
    }
    if(mapPut(chess->tournaments,tournament_id,new_tournament)!= MAP_SUCCESS){
        return CHESS_OUT_OF_MEMORY;//Already checked NULL arguments, so its has to be memory failure.
    }
    return CHESS_SUCCESS;
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player,
                         int second_player, Winner winner, int play_time)
{
    if (chess == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id < 0 || first_player < 0 || second_player < 0 || first_player == second_player) {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments, tournament_id)) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Tournament* tournament = mapGet(chess->tournaments, tournament_id);
    assert(tournament != NULL);
    if (tournament-> winner != -1) {
        return CHESS_TOURNAMENT_ENDED;
    }
    if(checkAlreadyPlayed(tournament,first_player,second_player)){
        return CHESS_GAME_ALREADY_EXISTS;
    }
    if (play_time < 0) {
        return CHESS_INVALID_PLAY_TIME;
    }
    if (checkExeededGames(tournament, first_player) || checkExeededGames(tournament, second_player)){
        return CHESS_EXCEEDED_GAMES;
    }
    Game *new_game = gameCreate(first_player, second_player, winner, play_time);
    if (new_game == NULL) {
        return CHESS_OUT_OF_MEMORY;
    }
    if (mapPut(tournament, tournament->next_game_id++, new_game) != MAP_SUCCESS) {
        return CHESS_OUT_OF_MEMORY; //Already checked NULL arguments, so its has to be memory failure.
    }
    return CHESS_SUCCESS;
}

ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{
    if (chess == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id < 0) {
        return CHESS_INVALID_ID;
    }
    if (!mapContains(chess->tournaments, tournament_id)) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    MapResult result = mapRemove(chess->tournaments, tournament_id);
    assert(result == MAP_SUCCESS);
    //+++++++++++ NEED TO UPDATE STATISTICS!!!!! +++++++++++
    //=======================================================

    return CHESS_SUCCESS;
}
ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
    if(chess == NULL){
        return CHESS_NULL_ARGUMENT;
    }
    if(player_id <= 0){
        return CHESS_INVALID_ID;
    }
    int count=0;
    MAP_FOREACH(int *,iter,chess->tournaments){
        Tournament* curr_tournament=(Tournament*)mapGet(chess->tournaments,iter);
        if(curr_tournament->winner == TOURNAMENT_NOT_ENDED){
            count += tournamentRemovePlayer(curr_tournament,player_id);
        }
    }
    if(!count){
        return CHESS_PLAYER_NOT_EXIST;
    }
    return CHESS_SUCCESS;
}


MapKeyElement copyKeyInt(MapKeyElement n) {
    if (!n) {
        return NULL;
    }
    int *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = *(int *) n;
    return copy;
}

void freeInt(MapKeyElement n) {
    free(n);
}

int compareInts(MapKeyElement n1, MapKeyElement n2) {
    return (*(int *) n1 - *(int *) n2);
}



int main() 
{
    return 0;
}