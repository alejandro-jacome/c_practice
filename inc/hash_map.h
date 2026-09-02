#ifndef HASHMAP_H 
#define HASHMAP_H

#include <stddef.h>
#include <stdbool.h>

// --- Allocator struct ---
typedef struct Allocator {
    void* (*alloc)(size_t size, void* context);
    void (*free)(void* ptr, void* context);
    void* context;
} Allocator;

void* default_alloc(size_t size, void* context);
void default_free(void* ptr, void* context);

#define DEFAULT_ALLOCATOR (Allocator){  \
    .alloc = default_alloc,             \
    .free = default_free,               \
    .context = NULL                     \
}


// --- Function Pointers ---

// Takes a generic key and turns it into a large integer.
typedef size_t (*HashFunc)(const void* key);

// Takes two generic keys and returns true if they are identical.
typedef bool (*EqualsFunc)(const void* key_a, const void* key_b);


// --- Core Structures ---

// Standardized error codes
typedef enum {
    HASHMAP_OK = 0,
    HASHMAP_ERR_OOM = 1, // Out of Memory
} HashMap_Error;

// The main Hash Map structure
typedef struct {
    size_t count;       // How many active items are currently in the map
    size_t capacity;    // How much total space is currently allocated
    
    HashFunc hash1;     // Primary hashing logic (determines initial bucket)
    HashFunc hash2;     // Secondary hashing logic (determines probe step size)
    EqualsFunc equals;  // The user's comparison logic
    Allocator alloc;    // The user's memory allocator
    
    void* buckets;      // Opaque pointer hiding your internal array/bucket design
} HashMap;

// Initializes the map with the user's custom logic and allocator.
// You must set initial capacity and allocate your internal buckets here.
HashMap_Error HashMap_create(HashMap** out_map, HashFunc hash1, HashFunc hash2, EqualsFunc equals, Allocator alloc);

// Frees all internal memory used by the map using the user's allocator.
void HashMap_destroy(HashMap* map);

// Inserts a key-value pair. If the key already exists, overwrite the value.
// Triggers a resize if the load factor exceeds your defined threshold.
HashMap_Error HashMap_put(HashMap* map, void* key, void* value);

// Retrieves a value. Returns true if found, putting the result into `out_value`.
// Returns false if the key doesn't exist.
bool HashMap_get(HashMap* map, const void* key, void** out_value);

// Removes a key-value pair from the map. 
// Returns true if removed, false if not found.
bool HashMap_remove(HashMap* map, const void* key);

// A convenience function to check if a key exists without grabbing its value.
bool HashMap_contains(HashMap* map, const void* key);

#endif
