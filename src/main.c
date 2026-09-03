#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "inc/hash_map.h"

// Both of these functions will be used for hashing strings
// Got them from this website http://www.cse.yorku.ca/~oz/hash.html

// djb2
static size_t hash1(const void* key) {
    char* str = (char*)key;
    size_t hash = 5381;
    size_t c = 0;

    while ((c = (size_t)*str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

// sdbm
static size_t hash2(const void* key) {
    char* str = (char*)key;
    size_t hash = 0;
    size_t c;

    while ((c = (size_t)*str++)) {
            hash = c + (hash << 6) + (hash << 16) - hash;
    }
   return hash;
}

static bool equals(const void* key_a, const void* key_b) {
    return (strcmp(((char*)key_a), (char*)key_b)) ? true : false;
}

int main() {
    HashMap* map = NULL;
    HashMap_Error err = HashMap_create(&map, hash1, hash2, equals, DEFAULT_ALLOCATOR);
    if(err != HASHMAP_OK) {
        printf("Something went wrong");
        return -1;
    }
}
