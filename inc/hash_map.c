#include "hash_map.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// Constants
static const size_t DEFAULT_PER_TABLE_CAPACITY = 8;
static const float MAX_LOAD_FACTOR = 0.7f;

void* default_alloc(size_t size, void* context) {
    (void)context;
    return malloc(size);
}
void default_free(void* ptr, void* context) {
    (void)context;
    free(ptr);
}

typedef struct {
    void* key;
    void* value;
    bool is_occupied;
} Bucket;

HashMap_Error HashMap_create(HashMap** out_map, HashFunc hash1, HashFunc hash2, EqualsFunc equals, Allocator alloc) {
    HashMap* map = alloc.alloc(sizeof(HashMap), alloc.context);
    if(map == NULL) {
        return HASHMAP_ERR_OOM;
    }
    map->count = 0;
    map->capacity = DEFAULT_PER_TABLE_CAPACITY;
    map->hash1 = hash1;
    map->hash2 = hash2;
    map->equals = equals;
    map->alloc = alloc;

    map->table1 = map->alloc.alloc(sizeof(Bucket) * DEFAULT_PER_TABLE_CAPACITY, alloc.context);
    if(map->table1== NULL) {
        map->alloc.free(map, alloc.context); // We have to free the prev memory to avoid a memory leak
        return HASHMAP_ERR_OOM;
    }

    map->table2 = map->alloc.alloc(sizeof(Bucket) * DEFAULT_PER_TABLE_CAPACITY, alloc.context);
    if(map->table2== NULL) {
        map->alloc.free(map, alloc.context); // We have to free the prev memory to avoid a memory leak
        map->alloc.free(map->table1, map->alloc.context);
        return HASHMAP_ERR_OOM;
    }

    Bucket* buckets1= (Bucket*)map->table1;
    Bucket* buckets2= (Bucket*)map->table2;
    for(size_t i=0; i<DEFAULT_PER_TABLE_CAPACITY; i++) {
        buckets1[i] = (Bucket){NULL, NULL, false};
        buckets2[i] = (Bucket){NULL, NULL, false};
    }

    *out_map = map; // Here as assign the hash map we just created to the passed in pointer;
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
