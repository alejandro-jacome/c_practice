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

HashMap_Error HashMap_init(HashMap *map, HashFunc hash1, HashFunc hash2, EqualsFunc equals, Allocator alloc) {
    map = alloc.alloc(sizeof(HashMap), alloc.context);
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

    return HASHMAP_OK;
}
