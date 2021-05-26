#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "game.h"
#include "tournament.h"

#define CAPITAL_A 'A'
#define CAPITAL_Z 'Z'
#define SMALL_A 'a'
#define SMALL_Z 'z'
#define SPACE ' '
#define PARAMETERS 5
#define UNDEFINED -1



int *statsArrCopy(int );
static MapKeyElement copyStatistics(MapKeyElement statistics);
static void destroyStatistics(MapKeyElement statistics);
static int computeMaxScore(Map players_stats, int *winner);
static int computeMinLosses(Map players_stats, int *winner, int maxScore);
static int computeMaxWins(Map players_stats, int *winner, int maxScore, int minLosses);
static void computeMinId(Map players_stats, int *winner,int maxScore,int minLosses,int maxWins);
static Tournament* tournamentCopyData(Map games, Map players_stats,const char *location, int winner,
                                    int max_games_per_player,int next_game_id);
static void resetStats(Map player_stats,int player_id);
static MapResult addPlayerStats(Map players_stats, int player_id);


Tournament* tournamentCreate(copyMapKeyElements copyMapKeyElements,freeMapKeyElements freeMapKeyElements, 
                             compareMapKeyElements compareMapKeyElements, const char *location,int max_games_per_player)
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
    tournament->players_stats = mapCreate(copyStatistics,copyMapKeyElements,
                                destroyStatistics, freeMapKeyElements,
                                compareMapKeyElements);
     if (tournament->players_stats == NULL) {
        mapDestroy(tournament->games);
        free(tournament);
        return NULL;
    }
    tournament->location = malloc(sizeof(char) * strlen(location) + 1);
    if (tournament->location == NULL) {
        mapDestroy(tournament->games);
        mapDestroy(tournament->players_stats);
        free(tournament);
        return NULL;
    }
    strcpy(tournament->location, location);
    tournament->winner = TOURNAMENT_NOT_ENDED;
    tournament->max_games_per_player=max_games_per_player;
    tournament->next_game_id = 0;
    return tournament;
}
static Tournament* tournamentCopyData(Map games, Map players_stats, const char *location, int winner,
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
    tournament->players_stats = mapCopy(players_stats);
    if (tournament->players_stats == NULL) {
        mapDestroy(tournament->games);
        free(tournament);
        return NULL;
    }
    tournament->location = malloc(sizeof(char) * strlen(location) + 1);
    if (tournament->location == NULL) {
        mapDestroy(tournament->games);
        mapDestroy(tournament->players_stats);
        free(tournament);
        return NULL;
    }
    strcpy(tournament->location, location);
    tournament->winner = winner;
    tournament->next_game_id = next_game_id;
    tournament->max_games_per_player = max_games_per_player;
    return tournament;
}

MapDataElement tournamentCopy(MapDataElement tournament)
{
    if (tournament == NULL) {
        return NULL;
    }
    Tournament *tour = (Tournament*)tournament;
    Tournament* tournament_copy = tournamentCopyData(tour->games,
                                                    tour->players_stats,
                                                    tour->location,
                                                    tour->winner,
                                                    tour->max_games_per_player,
                                                    tour->next_game_id);
   
    return (void*)tournament_copy;
}

bool checkAlreadyPlayed(const Tournament* tournament, int player1, int player2)
{
    assert(tournament != NULL);
    MAP_FOREACH(int *, iter, tournament->games) {
        Game* game = (Game*)mapGet(tournament->games, iter);
        if (game->players_id[0] == player1 && game->players_id[1] == player2) {
            free(iter);
            return true;
        }
        if (game->players_id[0] == player2 && game->players_id[1] == player1) {
            free(iter);
            return true;
        }
        free(iter);
    }
    return false;
}

bool checkExceededGames(const Tournament* tournament, int player)
{
    assert(tournament != NULL);
    int count_games = 0;
    MAP_FOREACH(int *, iter, tournament->games) {
         Game* game = (Game*)mapGet(tournament->games, iter);
         if (game->players_id[0] == player || game->players_id[1] == player) {
             count_games++;
         }
         free(iter);
    }
    return count_games >= tournament->max_games_per_player;
}

void tournamentDestroy(MapDataElement tournament)
{
    if (tournament != NULL) {
        Tournament *tournament_to_destroy= (Tournament*)tournament;
        free(tournament_to_destroy->location);
        mapDestroy(tournament_to_destroy->games);
        mapDestroy(tournament_to_destroy->players_stats);
        free(tournament_to_destroy);
    }    
}

bool checkLocation(const char* tournament_location)
{
    if (tournament_location == NULL || strlen(tournament_location) == 0) {
        return false;
    }

    if (*tournament_location  < CAPITAL_A || *tournament_location > CAPITAL_Z) {
        return false;
    }
    tournament_location++;
    while (*tournament_location) {
        if (*tournament_location == SPACE || (*tournament_location >= SMALL_A && *tournament_location <= SMALL_Z)) {
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
    bool exists_in_tournament = false;
    MAP_FOREACH(int *, iter, tournament->games) {
        Game* game = (Game*)mapGet(tournament->games, iter);
        Winner curr_winner = game->result;
        if (game->players_id[0] == player_id){
            game->result= SECOND_PLAYER;
            game->players_id[0]= TOURNAMENT_DELETED_PLAYER;
            exists_in_tournament = true;
            if (game->players_id[1] != TOURNAMENT_DELETED_PLAYER) {
                if (curr_winner == DRAW) {
                    updateStats(tournament->players_stats, game->players_id[1], 1, 0, -1, 0);
                }
                else if (curr_winner == FIRST_PLAYER) {
                    updateStats(tournament->players_stats, game->players_id[1], 1, -1, 0, 0);
                }
            }
        }      
        if (game->players_id[1] == player_id){
            game->result= FIRST_PLAYER;
            game->players_id[1]= TOURNAMENT_DELETED_PLAYER;
            exists_in_tournament = true;
            if (game->players_id[0] != TOURNAMENT_DELETED_PLAYER) {
                if (curr_winner == DRAW) {
                    updateStats(tournament->players_stats, game->players_id[0], 1, 0, -1, 0);
                }
                else if (curr_winner == SECOND_PLAYER) {
                    updateStats(tournament->players_stats, game->players_id[0], 1, -1, 0, 0);
                }
            }
        }
        free(iter);
    }
    if (exists_in_tournament) {
        resetStats(tournament->players_stats, player_id);
    }
    return exists_in_tournament;
}

static void resetStats(Map player_stats,int player_id)
{
    assert(player_stats != NULL);
    int *statistics = mapGet(player_stats, &player_id);
    statistics[WINS] = 0;
    statistics[LOSSES] = 0;
    statistics[DRAWS] = 0;
    statistics[TIME_PLAYED] = 0;
    statistics[SCORE] = 0;
}

static MapKeyElement copyStatistics(MapKeyElement statistics)
{
    int *copy = malloc(sizeof(int) * PARAMETERS);
    if (copy == NULL) {
        return NULL;
    }
    for (int i = 0; i < PARAMETERS; i++) {
        copy[i] = ((int *)statistics)[i];
    }
    return (MapKeyElement)copy;
}

static void destroyStatistics(MapKeyElement statistics)
{
    free((int *)statistics);
}

MapResult updateStats(Map players_stats, int player_id, int wins, int losses, int draws, int time_played)
{
    if (mapContains(players_stats, &player_id) == false) {
        MapResult result = addPlayerStats(players_stats, player_id);
        if (result != MAP_SUCCESS) {
            return result;
        }
    }
    int *player = (int*)mapGet(players_stats, &player_id);
    player[WINS] += wins;
    player[LOSSES] += losses;
    player[DRAWS] += draws;
    player[TIME_PLAYED] += time_played;
    player[SCORE] += 2 * wins +  draws;
    return MAP_SUCCESS;
}

static MapResult addPlayerStats(Map players_stats, int player_id)
{
    int *stats = calloc(PARAMETERS, sizeof(int));
    if (stats == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    if (mapPut(players_stats, &player_id, stats) != MAP_SUCCESS) {
        return MAP_OUT_OF_MEMORY; //Already checked NULL arguments, so its has to be memory failure.
    }
    destroyStatistics(stats);
    return MAP_SUCCESS;
}

void tournamentEnd(Tournament* tournament)
{
    assert(tournament != NULL);
    int winner = UNDEFINED;
    int max_score = computeMaxScore(tournament->players_stats, &winner);
    if (winner != UNDEFINED) {
        tournament->winner = winner;
        return;
    }
    int min_losses = computeMinLosses(tournament->players_stats, &winner, max_score);
    if (winner != UNDEFINED) {
        tournament->winner = winner;
        return;
    }
    int max_wins = computeMaxWins(tournament->players_stats, &winner, max_score, min_losses);
    if (winner != UNDEFINED) {
        tournament->winner = winner;
        return;
    }
    computeMinId(tournament->players_stats, &winner, max_score, min_losses, max_wins);
    assert(winner != UNDEFINED);
    tournament->winner = winner;
}

static int computeMaxScore(Map players_stats, int *winner)
{
    int maxScore=0;
    int count=0;
    MAP_FOREACH(int *, iter, players_stats) {
        int* player = (int*)mapGet(players_stats, iter);
        if(player[SCORE] > maxScore){
            maxScore=player[SCORE];
            *winner=*iter;
            count=0;
        }
        if(player[SCORE] == maxScore){
            count++;
        }
        free(iter);
    }
    if(count > 1){
        *winner=UNDEFINED;
    }
    return maxScore;
    
}
static int computeMinLosses(Map players_stats, int *winner, int maxScore)
{
    int minLosses=-1;
    int count=0;
    MAP_FOREACH(int *, iter, players_stats) {
        int* player = (int*)mapGet(players_stats, iter);
        if(maxScore != player[SCORE]){
            free(iter);
            continue;
        }
        if(minLosses == -1){
            minLosses = player[LOSSES];
        }
        if(player[LOSSES] < minLosses){
            minLosses=player[LOSSES];
            *winner=*iter;
            count=0;
        }
        if(player[SCORE] == minLosses){
            count++;
        }
        free(iter);
    }
    if(count > 1){
        *winner=UNDEFINED;
    }
    return minLosses;
}
static int computeMaxWins(Map players_stats, int *winner, int maxScore, int minLosses)
{
    int maxWins=0;
    int count=0;
    MAP_FOREACH(int *, iter, players_stats) {
        int* player = (int*)mapGet(players_stats, iter);
        if(maxScore != player[SCORE] || minLosses != player[LOSSES]){
            free(iter);
            continue;
        }
        if(player[WINS] > maxWins){
            maxWins=player[WINS];
            *winner=*iter;
            count=0;
        }
        if(player[WINS] == maxWins){
            count++;
        }
        free(iter);
    }
    if(count > 1){
        *winner=UNDEFINED;
    }
    return maxWins;
}
static void computeMinId(Map players_stats, int *winner,int maxScore,int minLosses,int maxWins)
{
    MAP_FOREACH(int *, iter, players_stats) {
        int* player = (int*)mapGet(players_stats, iter);
        if(maxScore == player[SCORE] && minLosses == player[LOSSES] && maxWins == player[WINS]){
            *winner=*iter;
            free(iter);
            return;//Map keys are sorted from min to max, so when we find the first its the minimum.
        }
        free(iter);
    }
    assert(*winner != UNDEFINED);//We should'nt get here.
}