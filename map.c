#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "map.h"


#define INIT_SIZE 1
#define EXPAND_FACTOR 2
typedef struct Map_t {
    MapKeyElement *keys;
    MapDataElement *data;
    int size;
    int max_size;
    int iterator;

    copyMapDataElements copyDataElement;
    copyMapKeyElements copyKeyElement;
    freeMapDataElements freeDataElement;
    freeMapKeyElements freeKeyElement;
    compareMapKeyElements compareKeyElements;
} Map_t;

static void copy_arr(void **src, void **dest, int size, void*(*copyFunction)(void*), MapResult *result);
static int mapGetIndex(Map map, MapKeyElement keyElement);
static MapResult mapExpand(Map map);
static void mapAdd(Map map,MapKeyElement keyElement,MapDataElement new_data);

Map mapCreate(copyMapDataElements copyDataElement,
              copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,
              freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements)
{
    if (!copyDataElement || !copyKeyElement || !freeDataElement || !freeKeyElement || !compareKeyElements) {
        return NULL;
    }

    Map map = malloc(sizeof(*map));
    if (map == NULL) {
        return NULL;
    }
    map->keys = malloc(sizeof(MapKeyElement) * INIT_SIZE);
    if (map->keys == NULL) {
        free(map);
        return NULL;
    }
    map->data = malloc(sizeof(MapDataElement) * INIT_SIZE);
    if (map->data == NULL) {
        free(map->keys);
        free(map);
        return NULL;
    }
    map->size = 0;
    map->max_size = 1;
    map->iterator = 0;

    map->copyDataElement = copyDataElement;
    map->copyKeyElement = copyKeyElement;
    map->freeDataElement = freeDataElement;
    map->freeKeyElement = freeKeyElement;
    map->compareKeyElements = compareKeyElements;

    return map;
}

void mapDestroy(Map map)
{
    if (map == NULL) {
        return;
    }
    mapClear(map);
    free(map->keys);
    free(map->data);
    free(map);
}

static void copy_arr(void **src, void **dest, int size, void*(*copyFunction)(void*), MapResult *result)
{
    assert(src && dest);
    for (int i = 0; i < size; i++) {
        dest[i] = copyFunction(src[i]);
        if (dest[i] == NULL) {
            *result = MAP_COPY_FAILURE;
            return;
        }
    }
    *result = MAP_SUCCESS;
}

Map mapCopy(Map map)
{
    if (map == NULL) {
        return NULL;
    }
    Map copy = mapCreate(map->copyDataElement,
                        map->copyKeyElement,
                        map->freeDataElement,
                        map->freeKeyElement,
                        map->compareKeyElements);
    if (copy == NULL) {
        return NULL;
    }
    copy->keys = realloc(map->keys, sizeof(MapKeyElement) * map->max_size);
    if (copy->keys == NULL) {
        mapDestroy(copy);
        return NULL;
    }
    copy->data = realloc(map->data, sizeof(MapDataElement) * map->max_size);
    if (copy->data == NULL) {
        mapDestroy(copy);
        return NULL;
    }
    
    MapResult result;
    copy_arr(map->keys, copy->keys, map->size, map->copyKeyElement, &result);
    if (result != MAP_SUCCESS) {
        mapDestroy(copy);
        return NULL;
    }
    copy_arr(map->data, copy->data, map->size, map->copyDataElement, &result);
    if (result != MAP_SUCCESS) {
        mapDestroy(copy);
        return NULL;
    }

    copy->size = map->size;
    copy->max_size = map->max_size;
    copy->iterator = map->iterator;

    copy->copyDataElement = map->copyDataElement;
    copy->copyKeyElement = map->copyKeyElement;
    copy->freeDataElement = map->freeDataElement;
    copy->freeKeyElement = map->freeKeyElement;
    copy->compareKeyElements = map->compareKeyElements;

    return copy;
}

int mapGetSize(Map map)
{
    if (map == NULL) {
        return -1;
    }
    return map->size;
}

bool mapContains(Map map, MapKeyElement element)
{
    if (map == NULL || element == NULL) {
        return false;
    }

    int i = mapGetIndex(map,element );
    if (i != -1) {
        return true;
    }
    return false;
}

MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
     if (!map || !keyElement) {
         return NULL;
     }
     
     int i = mapGetIndex(map, keyElement);
     if (i != -1) {
         return map->data[i];
     }
     return NULL;
}

static int mapGetIndex(Map map, MapKeyElement keyElement)
{
    assert(map && keyElement);
    for (int i = 0; i < map->size; i++) {
        if (map->compareKeyElements(map->keys[i], keyElement) == 0) {
            return i;
        }
    }
    return -1;
}
static MapResult mapExpand(Map map)
{
    int new_size = EXPAND_FACTOR * map->max_size;
    map->keys = realloc(map->keys, sizeof(MapKeyElement) * new_size);
    if (map->keys == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    map->data = realloc(map->data, sizeof(MapDataElement) * new_size);
    if (map->data == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    map->max_size=new_size;
    return MAP_SUCCESS;
}
static void mapAdd(Map map,MapKeyElement keyElement,MapDataElement new_data)
{
    int i;
    for(i=0; i < map->size; i++){
        if(map->compareKeyElements(keyElement,map->keys[i]) < 0){
            break;
        }
    }
    for(int j=map->size; j > i; j--){ //shifts elements one index to the right
        map->keys[j]= map->keys[j-1];
        map->data[j]= map->data[j-1];
    }
    map->keys[i]=keyElement;
    map->data[i]=new_data;
}

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    if (!map || !keyElement || !dataElement) {
        return MAP_NULL_ARGUMENT;
    }
    MapDataElement new_data = map->copyDataElement(dataElement);
    if (new_data == NULL) {
        return MAP_OUT_OF_MEMORY;
    }
    int i = mapGetIndex(map, keyElement);
    if (i != -1) {
        MapDataElement old_data = mapGet(map, keyElement);
        map->freeDataElement(old_data);
        map->keys[i] = new_data;
        return MAP_SUCCESS;
    }
    else {
        if(map->size == map->max_size){
            if(mapExpand(map)==MAP_OUT_OF_MEMORY){
                return MAP_OUT_OF_MEMORY;
            }
        }
        MapKeyElement copy_key = map->copyKeyElement(keyElement);
        if (copy_key == NULL) {
            return MAP_OUT_OF_MEMORY;
        }
        mapAdd(map, copy_key, new_data);
        map->size++;
        return MAP_SUCCESS;
    }
    
}

MapResult mapRemove(Map map, MapKeyElement keyElement)
{
    if(map == NULL || keyElement == NULL){
        return MAP_NULL_ARGUMENT;
    }
    int i=mapGetIndex(map,keyElement);
    if(i==-1){
        return MAP_ITEM_DOES_NOT_EXIST;
    }
    assert(i>=0);
    map->freeKeyElement(map->keys[i]);
    map->freeDataElement(map->data[i]);
    for(int j=i; j < map->size-1; j++){ //shifts elememnts one index to the left
        map->keys[j]=map->keys[j+1];
        map->data[j]=map->data[j+1];
    }
    map->size--;
    return MAP_SUCCESS;
    
}

MapKeyElement mapGetFirst(Map map)
{
    if(map == NULL){
        return NULL;
    }
    map->iterator = 0;
    return mapGetNext(map);
}

MapKeyElement mapGetNext(Map map) 
{
    if(map==NULL){
        return NULL;
    }
    
    if (map->iterator >= map->size) {
        return NULL;
    }
 return map->keys[map->iterator++];
}

MapResult mapClear(Map map)
{
    if(map == NULL){
        return MAP_NULL_ARGUMENT;
    }
    for (int i = 0; i < map->size; i++) {
        map->freeKeyElement(map->keys[i]);
        map->freeDataElement(map->data[i]);
    }
    return MAP_SUCCESS;
}

