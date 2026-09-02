#include "hash_map.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// Constants
static const size_t DEFAULT_CAPACITY = 16;
static const float MAX_LOAD_FACTOR = 0.7f;

void* default_alloc(size_t size, void* context) {
    (void)context;
    return malloc(size);
}
void default_free(void* ptr, void* context) {
    (void)context;
    free(ptr);
}

typedef enum {
    Dead = -1,
    Full = 0,
    Empty = 1,
} BucketState;

typedef struct {
    BucketState state;
    void* key;
    void* value;
} Bucket;

HashMap_Error HashMap_create(HashMap** out_map, HashFunc hash1, HashFunc hash2, EqualsFunc equals, Allocator alloc) {
    HashMap* map = alloc.alloc(sizeof(HashMap), alloc.context);
    if(map == NULL) {
        return HASHMAP_ERR_OOM;
    }
    map->count = 0;
    map->capacity = DEFAULT_CAPACITY;
    map->hash1 = hash1;
    map->hash2 = hash2;
    map->equals = equals;
    map->alloc = alloc;
    map->buckets = map->alloc.alloc(sizeof(Bucket) * DEFAULT_CAPACITY, alloc.context);

    if(map->buckets == NULL) {
        map->alloc.free(map, alloc.context); // We have to free the prev memory to avoid a memory leak
        return HASHMAP_ERR_OOM;
    }

    Bucket* bucket = (Bucket*)map->buckets;
    for(size_t i=0; i<DEFAULT_CAPACITY; i++) {
        bucket[i] = (Bucket){Empty, NULL, NULL};
    }
    *out_map = map;
    return HASHMAP_OK;
}

HashMap_Error HashMap_put(HashMap *map, void *key, void *value) {
    // Ok so first we wanna create the new map duh
    Bucket* bucket = map->alloc.alloc(sizeof(Bucket), map->alloc.context);
    if(bucket == NULL) {
        return HASHMAP_ERR_OOM;
    }
    // Then we put it's resouces in
    *bucket = (Bucket){Full, key, value};

    // Now come the hard part

    return HASHMAP_OK;
}
