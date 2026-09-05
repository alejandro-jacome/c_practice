#include "hash_map.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>
// Constants
static const size_t DEFAULT_PER_TABLE_CAPACITY = 8;
static const float MAX_LOAD_FACTOR = 0.5f;

void *default_alloc(size_t size, void *context) {
    (void)context;
    return malloc(size);
}
void default_free(void *ptr, void *context) {
    (void)context;
    free(ptr);
}

typedef struct {
    void *key;
    void *value;
    bool is_occupied;
} Bucket;

size_t max_loops(size_t capacity) {
    size_t loops = (size_t)(3.0f * log2f((float)capacity));
    return (loops < 10) ? 10 : loops;
}

HashMap_Error HashMap_create(HashMap **out_map, HashFunc hash1, HashFunc hash2,
                             EqualsFunc equals, Allocator alloc) {
    HashMap *map = alloc.alloc(sizeof(HashMap), alloc.context);
    if (map == NULL) {
        return HASHMAP_ERR_OOM;
    }
    map->count = 0;
    map->capacity = DEFAULT_PER_TABLE_CAPACITY;
    map->hash1 = hash1;
    map->hash2 = hash2;
    map->equals = equals;
    map->alloc = alloc;

    map->table1 = map->alloc.alloc(sizeof(Bucket) * DEFAULT_PER_TABLE_CAPACITY,
                                   alloc.context);
    if (map->table1 == NULL) {
        map->alloc.free(map, alloc.context); // We have to free the prev memory
                                             // to avoid a memory leak
        return HASHMAP_ERR_OOM;
    }

    map->table2 = map->alloc.alloc(sizeof(Bucket) * DEFAULT_PER_TABLE_CAPACITY,
                                   alloc.context);
    if (map->table2 == NULL) {
        map->alloc.free(map->table1, map->alloc.context);
        map->alloc.free(map, alloc.context); // We have to free the prev memory
                                             // to avoid a memory leak
        return HASHMAP_ERR_OOM;
    }

    Bucket *buckets1 = (Bucket *)map->table1;
    Bucket *buckets2 = (Bucket *)map->table2;
    for (size_t i = 0; i < DEFAULT_PER_TABLE_CAPACITY; i++) {
        buckets1[i] = (Bucket){NULL, NULL, false};
        buckets2[i] = (Bucket){NULL, NULL, false};
    }

    *out_map = map; // Here as assign the hash map we just created to the passed
                    // in pointer;
    return HASHMAP_OK;
}

HashMap_Error HashMap_put(HashMap *map, void *key, void *value) {
    // Check capacity first
    if ((float)map->count / (float)(2 * map->capacity) >= MAX_LOAD_FACTOR) {
        HashMap_Error err = HashMap_rehash(map);
        if (err != HASHMAP_OK) {
            return err;
        }
    }

    Bucket *buckets1 = (Bucket *)map->table1;
    size_t idx1 = map->hash1(key) % map->capacity;
    if (buckets1[idx1].is_occupied && map->equals(buckets1[idx1].key, key)) {
        buckets1[idx1].value = value;
        return HASHMAP_OK;
    }

    Bucket *buckets2 = (Bucket *)map->table2;
    size_t idx2 = map->hash2(key) % map->capacity;
    if (buckets2[idx2].is_occupied && map->equals(buckets2[idx2].key, key)) {
        buckets2[idx2].value = value;
        return HASHMAP_OK;
    }

    size_t i = 0;
    while (i < max_loops(map->capacity)) {
        // If T1(hash1(key)) is not occupied we place it there
        idx1 = map->hash1(key) % map->capacity;
        if (!buckets1[idx1].is_occupied) {
            buckets1[idx1] = (Bucket){key, value, true};
            map->count++;
            return HASHMAP_OK;
        }
        // If it is occupied, we place the key and value we were trying to place
        // in that spot
        void *temp_key = buckets1[idx1].key;
        void *temp_value = buckets1[idx1].value;
        buckets1[idx1] = (Bucket){key, value, true};
        key = temp_key;
        value = temp_value;

        idx2 = map->hash2(key) % map->capacity;
        if (!buckets2[idx2].is_occupied) {
            buckets2[idx2] = (Bucket){key, value, true};
            map->count++;
            return HASHMAP_OK;
        }
        temp_key = buckets2[idx2].key;
        temp_value = buckets2[idx2].value;
        buckets2[idx2] = (Bucket){key, value, true};
        key = temp_key;
        value = temp_value;

        i++;
    }
    HashMap_Error err = HashMap_rehash(map);
    if (err != HASHMAP_OK) {
        return err;
    }

    err = HashMap_put(map, key, value);
    if (err != HASHMAP_OK) {
        return err;
    }

    return HASHMAP_OK;
}

HashMap_Error HashMap_rehash(HashMap *map) {
    size_t new_capacity = map->capacity * 2;
    size_t old_capacity = map->capacity;
    size_t old_count = map->count;

    void *new_table1 =
        map->alloc.alloc(sizeof(Bucket) * new_capacity, map->alloc.context);
    if (new_table1 == NULL) {
        return HASHMAP_ERR_OOM;
    }

    void *new_table2 =
        map->alloc.alloc(sizeof(Bucket) * new_capacity, map->alloc.context);
    if (new_table2 == NULL) {
        map->alloc.free(new_table1, map->alloc.context);
        return HASHMAP_ERR_OOM;
    }
    void *old_table1 = map->table1;
    void *old_table2 = map->table2;
    Bucket *old_buckets1 = (Bucket *)map->table1;
    Bucket *old_buckets2 = (Bucket *)map->table2;

    map->table1 = new_table1;
    map->table2 = new_table2;
    map->capacity = new_capacity;
    map->count = 0;

    Bucket *new_buckets1 = (Bucket *)new_table1;
    Bucket *new_buckets2 = (Bucket *)new_table2;
    for (size_t i = 0; i < new_capacity; i++) {
        new_buckets1[i] = (Bucket){NULL, NULL, false};
        new_buckets2[i] = (Bucket){NULL, NULL, false};
    }

    for (size_t i = 0; i < old_capacity; i++) {
        if (old_buckets1[i].is_occupied) {
            HashMap_Error err =
                HashMap_put(map, old_buckets1[i].key, old_buckets1[i].value);
            if (err != HASHMAP_OK) {
                // ROLLBACK: Restore the map to its original state
                map->table1 = old_table1;
                map->table2 = old_table2;
                map->capacity = old_capacity;
                map->count = old_count;
                map->alloc.free(new_table1, map->alloc.context);
                map->alloc.free(new_table2, map->alloc.context);
                return err;
            }
        }

        if (old_buckets2[i].is_occupied) {
            HashMap_Error err =
                HashMap_put(map, old_buckets2[i].key, old_buckets2[i].value);
            if (err != HASHMAP_OK) {
                // ROLLBACK: Restore the map to its original state
                map->table1 = old_table1;
                map->table2 = old_table2;
                map->capacity = old_capacity;
                map->count = old_count;
                map->alloc.free(new_table1, map->alloc.context);
                map->alloc.free(new_table2, map->alloc.context);
                return err;
            }
        }
    }

    map->alloc.free(old_table1, map->alloc.context);
    map->alloc.free(old_table2, map->alloc.context);

    return HASHMAP_OK;
}

bool HashMap_contains(HashMap *map, const void *key) {
    size_t idx1 = map->hash1(key) % map->capacity;
    Bucket *bucket1 = &((Bucket *)map->table1)[idx1];

    size_t idx2 = map->hash2(key) % map->capacity;
    Bucket *bucket2 = &((Bucket *)map->table2)[idx2];

    return (bucket1->is_occupied && map->equals(key, bucket1->key)) ||
           (bucket2->is_occupied && map->equals(key, bucket2->key));
}

bool HashMap_get(HashMap *map, const void *key, void **out_value) {
    size_t idx1 = map->hash1(key) % map->capacity;
    Bucket *bucket1 = &((Bucket *)map->table1)[idx1];

    size_t idx2 = map->hash2(key) % map->capacity;
    Bucket *bucket2 = &((Bucket *)map->table2)[idx2];

    if (bucket1->is_occupied && map->equals(key, bucket1->key)) {
        *out_value = bucket1->value;
        return true;
    } else if (bucket2->is_occupied && map->equals(key, bucket2->key)) {
        *out_value = bucket2->value;
        return true;
    }

    return false;
}

bool HashMap_remove(HashMap *map, const void *key) {
    size_t idx1 = map->hash1(key) % map->capacity;
    Bucket *bucket1 = &((Bucket *)map->table1)[idx1];

    size_t idx2 = map->hash2(key) % map->capacity;
    Bucket *bucket2 = &((Bucket *)map->table2)[idx2];

    if (bucket1->is_occupied && map->equals(key, bucket1->key)) {
        bucket1->is_occupied = false;
        bucket1->key = NULL;
        bucket1->value = NULL;
        map->count--;
        return true;
    } else if (bucket2->is_occupied && map->equals(key, bucket2->key)) {
        bucket2->is_occupied = false;
        bucket2->key = NULL;
        bucket2->value = NULL;
        map->count--;
        return true;
    }
    return false;
}

void HashMap_destroy(HashMap *map) {
    if (map == NULL) {
        return;
    }

    if (map->table1 != NULL) {
        map->alloc.free(map->table1, map->alloc.context);
    }

    if (map->table2 != NULL) {
        map->alloc.free(map->table2, map->alloc.context);
    }

    map->alloc.free(map, map->alloc.context);
}
