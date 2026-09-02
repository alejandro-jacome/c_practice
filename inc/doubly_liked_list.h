#ifndef DLL_H 
#define DLL_H

#include <stddef.h>

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

typedef struct DoublyLinkedList DoublyLinkedList;

DoublyLinkedList* DLL_create(Allocator allocator);
void DLL_insertFront(DoublyLinkedList* dll, int value);
void DLL_insertEnd(DoublyLinkedList* dll, int value);
void DLL_print(DoublyLinkedList* dll);
#endif
