#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "map.h"
#include "chessSystem.h"
#include "game.h"
#include "tournament.h"

#define NO_AVERAGE -1

typedef struct chess_system_t {
    Map tournaments;
} chess_system_t;

static ChessResult updateWinnerStats(Tournament* tournament, int first_player,
                         int second_player, Winner winner, int play_time);
static void printToFile(Map players_ranked, FILE* file, ChessResult* result);
static double FindMaxData(Map players_ranked, int *id);
static Map computePlayersRank(ChessSystem chess, ChessResult* chess_result);
static int computeGamesStats(Map games, double* avg_game_time);
static ChessResult printTournamnentStats(Tournament* tournament, FILE* file);
MapKeyElement copyKeyInt(MapKeyElement n);
void freeInt(MapKeyElement n);
MapKeyElement copyDouble(MapKeyElement n);
int compareInts(MapKeyElement n1, MapKeyElement n2);
void freeDouble(MapKeyElement n);


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
    if (chess == NULL || tournament_location == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    if (tournament_id <= 0) {
        return CHESS_INVALID_ID;
    }
    if(mapContains(chess->tournaments, &tournament_id)){
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }
    if(!checkLocation(tournament_location)) {
        return CHESS_INVALID_LOCATION;
    }
    if (max_games_per_player <= 0) {
        return CHESS_INVALID_MAX_GAMES;
    }
    Tournament *new_tournament=tournamentCreate(copyKeyInt,
                                               freeInt,
                                               compareInts,
                                               tournament_location,
                                               max_games_per_player);
    if(new_tournament==NULL){
        return CHESS_OUT_OF_MEMORY;
    }
    if(mapPut(chess->tournaments, &tournament_id, new_tournament)!= MAP_SUCCESS){
        return CHESS_OUT_OF_MEMORY;//Already checked NULL arguments, so its has to be memory failure.
    }
    tournamentDestroy(new_tournament);
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
    if (!mapContains(chess->tournaments, &tournament_id)) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Tournament* tournament = mapGet(chess->tournaments, &tournament_id);
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
    if (checkExceededGames(tournament, first_player) || checkExceededGames(tournament, second_player)){
        return CHESS_EXCEEDED_GAMES;
    }
    Game *new_game = gameCreate(first_player, second_player, winner, play_time);
    if (new_game == NULL) {
        return CHESS_OUT_OF_MEMORY;
    }
    if (mapPut(tournament->games, &(tournament->next_game_id), new_game) != MAP_SUCCESS) {
        return CHESS_OUT_OF_MEMORY; //Already checked NULL arguments, so its has to be memory failure.
    }
    tournament->next_game_id++;
    gameDestroy(new_game);
    return updateWinnerStats(tournament, first_player, second_player, winner, play_time);
}

static ChessResult updateWinnerStats(Tournament* tournament, int first_player,
                         int second_player, Winner winner, int play_time)
{
    if (winner == FIRST_PLAYER) {
        if (updateStats(tournament->players_stats, first_player, 1, 0, 0, play_time) != MAP_SUCCESS) {
            return CHESS_OUT_OF_MEMORY;
        }
        if (updateStats(tournament->players_stats, second_player, 0, 1, 0, play_time) != MAP_SUCCESS) {
            return CHESS_OUT_OF_MEMORY;
        } 
    }
    else if (winner == SECOND_PLAYER) {
        if (updateStats(tournament->players_stats, first_player, 0, 1, 0, play_time) != MAP_SUCCESS) {
            return CHESS_OUT_OF_MEMORY;
        }
        if (updateStats(tournament->players_stats, second_player, 1, 0, 0, play_time) != MAP_SUCCESS) {
             return CHESS_OUT_OF_MEMORY;
        }
    }
    else {
        if (updateStats(tournament->players_stats, first_player, 0, 0, 1, play_time) != MAP_SUCCESS) {
             return CHESS_OUT_OF_MEMORY;
        }
        if (updateStats(tournament->players_stats, second_player, 0, 0, 1, play_time) != MAP_SUCCESS) {
            return CHESS_OUT_OF_MEMORY;
        }
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
    if (!mapContains(chess->tournaments, &tournament_id)) {
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    mapRemove(chess->tournaments, &tournament_id);
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
        if (mapContains(curr_tournament->players_stats, &player_id)) {
            count += ((int*)mapGet(curr_tournament->players_stats,&player_id))[TIME_PLAYED] > 0;
        }
        if(curr_tournament->winner == TOURNAMENT_NOT_ENDED){
            tournamentRemovePlayer(curr_tournament,player_id);
        }
        free(iter);
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
MapKeyElement copyDouble(MapKeyElement n) {
    if (!n) {
        return NULL;
    }
    double *copy = malloc(sizeof(*copy));
    if (!copy) {
        return NULL;
    }
    *copy = *(double *) n;
    return copy;
}

int compareInts(MapKeyElement n1, MapKeyElement n2) {
    return (*(int *) n1 - *(int *) n2);
}

void freeDouble(MapKeyElement n) {
    free(n);
}

ChessResult chessEndTournament (ChessSystem chess, int tournament_id)
{
    if(chess == NULL){
        return CHESS_NULL_ARGUMENT;
    }
    if(tournament_id < 0){
        return CHESS_INVALID_ID;
    }
    if(!mapContains(chess->tournaments, &tournament_id)){
        return CHESS_TOURNAMENT_NOT_EXIST;
    }
    Tournament* curr_tournament = mapGet(chess->tournaments, &tournament_id);
    if(curr_tournament->winner != TOURNAMENT_NOT_ENDED){
        return CHESS_TOURNAMENT_ENDED;
    }
    if(mapGetSize(curr_tournament->games) == 0){
        return CHESS_NO_GAMES;
    }
    tournamentEnd(curr_tournament);
    return CHESS_SUCCESS;
}
double chessCalculateAveragePlayTime (ChessSystem chess, int player_id, ChessResult* chess_result)
{
    if(chess == NULL){
        *chess_result = CHESS_NULL_ARGUMENT;
        return NO_AVERAGE;
    }
    if(player_id <= 0){
        *chess_result =  CHESS_INVALID_ID;
        return NO_AVERAGE;
    }
    int sum =0;
    int count=0;
    MAP_FOREACH(int *,iter,chess->tournaments){
        Tournament* curr_tournament=(Tournament*)mapGet(chess->tournaments,iter);
        if(mapContains(curr_tournament->players_stats, &player_id)){
            int *data=mapGet(curr_tournament->players_stats, &player_id);
            sum += data[TIME_PLAYED];
            count += data[WINS] + data[LOSSES] + data[DRAWS];
        }
        free(iter);
    }
    if(!count){
        *chess_result = CHESS_PLAYER_NOT_EXIST;
        return NO_AVERAGE;
    }
    *chess_result = CHESS_SUCCESS;
    return ((double)sum)/count;
}
ChessResult chessSavePlayersLevels (ChessSystem chess, FILE* file)
{
    if(chess == NULL || file == NULL){
        return CHESS_NULL_ARGUMENT;
    }
    ChessResult chess_result;
    Map players_ranked=computePlayersRank(chess, &chess_result);
    if(chess_result == CHESS_OUT_OF_MEMORY){
    return CHESS_OUT_OF_MEMORY;
    }
    printToFile(players_ranked, file, &chess_result);
    if(chess_result == CHESS_SAVE_FAILURE){
        return CHESS_SAVE_FAILURE;
    }
    mapDestroy(players_ranked);
    return CHESS_SUCCESS;
}
static void printToFile(Map players_ranked, FILE* file, ChessResult* result)
{
    int id=-1;
    while(mapGetSize(players_ranked) > 0)
    {
        double curr_max = FindMaxData(players_ranked, &id);
        assert(id != -1);
        if (fprintf(file, "%d %0.2f\n",id, curr_max) < 0) {
            *result = CHESS_SAVE_FAILURE;
            return;
        }
    }
}
static double FindMaxData(Map players_ranked, int *id)
{
    assert(players_ranked != NULL);
    int *first_id = (int*)mapGetFirst(players_ranked); //Initiallize first max to the first player
    double max = *(double*)mapGet(players_ranked, first_id); 
    int maxid = *first_id;
    free(first_id);
    MAP_FOREACH(int *,iter,players_ranked){
        double* level = (double*)mapGet(players_ranked, iter);
        if(*level > max){
            max = *level;
            maxid = *iter;
        }
        free(iter);
    }
    mapRemove(players_ranked, &maxid);
    *id = maxid;
    return max;
}
static Map computePlayersRank(ChessSystem chess, ChessResult* chess_result)
{
    Map players_ranked=mapCreate(copyDouble, copyKeyInt, freeDouble, freeInt ,compareInts);
    if(players_ranked == NULL){
        *chess_result = CHESS_OUT_OF_MEMORY;
        return NULL;
    }
    Map players_games_count=mapCreate(copyKeyInt, copyKeyInt, freeInt, freeInt ,compareInts);
    if(players_games_count == NULL){
        *chess_result = CHESS_OUT_OF_MEMORY;
        mapDestroy(players_ranked);
        return NULL;
    }
    MAP_FOREACH(int *,iter_tournaments,chess->tournaments){
        Tournament* curr_tournament=(Tournament*)mapGet(chess->tournaments,iter_tournaments);
        MAP_FOREACH(int *,iter_players,curr_tournament->players_stats){
            int *data=mapGet(curr_tournament->players_stats, iter_players);
            int num_games = data[WINS] + data[LOSSES] + data[DRAWS];
            if (num_games == 0) {
                free(iter_players);
                continue; //Deleted player - no games played
            }
            double rank = (double)(6 * data[WINS] - 10 * data[LOSSES] + 2 * data[DRAWS]);
            if(mapContains(players_ranked,iter_players)){
                double* curr_rank=mapGet(players_ranked,iter_players);
                int* curr_games=mapGet(players_games_count,iter_players);
                *curr_rank += rank;
                *curr_games += num_games;
            }
            else{
                if(mapPut(players_ranked,iter_players,&rank) != MAP_SUCCESS){
                    mapDestroy(players_ranked);
                    mapDestroy(players_games_count);
                    *chess_result = CHESS_OUT_OF_MEMORY;//It has to be memory error because iter_players and rank aren't NULL
                    free(iter_players);
                    free(iter_tournaments);
                    return NULL;
                }
                if(mapPut(players_games_count,iter_players,&num_games) != MAP_SUCCESS){
                    mapDestroy(players_ranked);
                    mapDestroy(players_games_count);
                    *chess_result = CHESS_OUT_OF_MEMORY;//It has to be memory error because iter_players and rank aren't NULL
                    free(iter_players);
                    free(iter_tournaments);
                    return NULL;
                }
            }
            free(iter_players);
        }
        free(iter_tournaments);
    }
    *chess_result = CHESS_SUCCESS;
    MAP_FOREACH(int *,iter_players,players_ranked){
        double *rank=mapGet(players_ranked, iter_players);
        int *num_of_games=mapGet(players_games_count, iter_players);
        *rank = *rank / *num_of_games;
        free(iter_players);
    }
    mapDestroy(players_games_count);
    return players_ranked;
}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
{
    if (chess == NULL || path_file == NULL) {
        return CHESS_NULL_ARGUMENT;
    }
    FILE *file = fopen(path_file, "w");
    if (file == NULL) {
        return CHESS_SAVE_FAILURE;
    }
    int count_ended_tournaments = 0;
    ChessResult result;
    MAP_FOREACH(int*, iter, chess->tournaments) {
        Tournament* tournament=mapGet(chess->tournaments, iter);
        if (tournament->winner == TOURNAMENT_NOT_ENDED) {
            free(iter);
            continue;
        }
        count_ended_tournaments++;
        result = printTournamnentStats(tournament, file);
        if (result == CHESS_SAVE_FAILURE) {
            fclose(file);
            free(iter);
            return result;
        }
        free(iter);
    }
    fclose(file);
    if (count_ended_tournaments == 0) {
        return CHESS_NO_TOURNAMENTS_ENDED;
    }
    return CHESS_SUCCESS;
}

static ChessResult printTournamnentStats(Tournament* tournament, FILE* file)
{
    assert(tournament != NULL);
    assert(file != NULL);
    if (fprintf(file, "%d\n",tournament->winner) < 0) {
        return CHESS_SAVE_FAILURE;
    } 
    double avg_game_time;
    int longest_game = computeGamesStats(tournament->games, &avg_game_time);
    if (fprintf(file, "%d\n%0.2f\n",longest_game, avg_game_time) < 0) {
        return CHESS_SAVE_FAILURE;
    }
    if (fprintf(file, "%s\n",tournament->location) < 0) {
        return CHESS_SAVE_FAILURE;
    } 
    if (fprintf(file, "%d\n",mapGetSize(tournament->games)) < 0) {
        return CHESS_SAVE_FAILURE;
    } 
    if (fprintf(file, "%d\n",mapGetSize(tournament->players_stats)) < 0) {
        return CHESS_SAVE_FAILURE;
    } 
    return CHESS_SUCCESS;
}

static int computeGamesStats(Map games, double* avg_game_time) {
    int max = 0, sum_duartion = 0;
    MAP_FOREACH(int*, iter, games) {
        Game* game=mapGet(games, iter);
        sum_duartion += game->duration;
        if (game->duration > max) {
            max = game->duration;
        }
        free(iter);
    }
    *avg_game_time = ((double)sum_duartion) / mapGetSize(games);
    return max;
}
