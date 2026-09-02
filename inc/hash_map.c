#include "hash_map.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

void* default_alloc(size_t size, void* context) {
    (void)context;
    return malloc(size);
}
void default_free(void* ptr, void* context) {
    (void)context;
    free(ptr);
}

