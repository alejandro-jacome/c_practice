#include "inc/hash_map.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Both of these functions will be used for hashing strings
// Got them from this website http://www.cse.yorku.ca/~oz/hash.html

// djb2
static size_t hash1(const void *key) {
    char *str = (char *)key;
    size_t hash = 5381;
    size_t c = 0;

    while ((c = (size_t)*str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

// sdbm
static size_t hash2(const void *key) {
    char *str = (char *)key;
    size_t hash = 0;
    size_t c;

    while ((c = (size_t)*str++)) {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash;
}

static bool equals(const void *key_a, const void *key_b) {
    return (strcmp(((char *)key_a), (char *)key_b)) == 0;
}

int main() {
    HashMap *map = NULL;
    HashMap_Error err =
        HashMap_create(&map, hash1, hash2, equals, DEFAULT_ALLOCATOR);
    if (err != HASHMAP_OK) {
        printf("create failed\n");
        return -1;
    }

    // --- Test 1: basic put/get ---
    HashMap_put(map, "apple", "red");
    HashMap_put(map, "banana", "yellow");
    HashMap_put(map, "grape", "purple");

    void *val;
    printf("apple -> %s\n",
           HashMap_get(map, "apple", &val) ? (char *)val : "MISSING");
    printf("banana -> %s\n",
           HashMap_get(map, "banana", &val) ? (char *)val : "MISSING");
    printf("kiwi -> %s\n",
           HashMap_get(map, "kiwi", &val) ? (char *)val : "MISSING (expected)");

    // --- Test 2: overwrite existing key ---
    HashMap_put(map, "apple", "green");
    printf("apple after overwrite -> %s\n",
           HashMap_get(map, "apple", &val) ? (char *)val : "MISSING");

    // --- Test 3: contains / remove ---
    printf("contains banana? %d\n", HashMap_contains(map, "banana"));
    printf("remove banana? %d\n", HashMap_remove(map, "banana"));
    printf("contains banana after remove? %d\n",
           HashMap_contains(map, "banana"));

    // --- Test 4: force rehashing with volume ---
    char keys[200][16];
    for (int i = 0; i < 200; i++) {
        snprintf(keys[i], sizeof(keys[i]), "key%d", i);
        HashMap_Error e = HashMap_put(
            map, keys[i], keys[i]); // value = key itself, easy to verify
        if (e != HASHMAP_OK) {
            printf("put failed at i=%d\n", i);
        }
    }

    // Verify every single one is retrievable and correct
    int failures = 0;
    for (int i = 0; i < 200; i++) {
        void *v;
        if (!HashMap_get(map, keys[i], &v) || strcmp((char *)v, keys[i]) != 0) {
            printf("MISMATCH at key%d\n", i);
            failures++;
        }
    }
    printf("volume test: %d failures out of 200\n", failures);

    // --- Test 5: remove half, re-check the rest are intact ---
    for (int i = 0; i < 200; i += 2) {
        HashMap_remove(map, keys[i]);
    }
    int wrongly_present = 0, wrongly_missing = 0;
    for (int i = 0; i < 200; i++) {
        bool present = HashMap_contains(map, keys[i]);
        if (i % 2 == 0 && present)
            wrongly_present++;
        if (i % 2 != 0 && !present)
            wrongly_missing++;
    }
    printf("after removing evens: wrongly_present=%d wrongly_missing=%d\n",
           wrongly_present, wrongly_missing);

    HashMap_destroy(map);
    return 0;
}
