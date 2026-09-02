#include "doubly_liked_list.h"
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

typedef struct Node {
    struct Node* next;
    struct Node* prev;
    int val;
} Node;

struct DoublyLinkedList {
    Node* head;
    Node* tail;
    size_t len;

    Allocator allocator;
};

DoublyLinkedList* DLL_create(Allocator allocator) {
    DoublyLinkedList* dll = (DoublyLinkedList*)allocator.alloc(sizeof(DoublyLinkedList), allocator.context);
    dll->head = NULL;
    dll->tail = NULL;
    dll->len = 0;
    dll->allocator = allocator;

    return dll;
}

void DLL_insertEnd(DoublyLinkedList* dll, int value) {
    Node* new_node = (Node*)dll->allocator.alloc(sizeof(Node), dll->allocator.context); 
    if(new_node == NULL) {
        fprintf(stderr, "Fatal: ran out of memory.\n");
        return;
    }

    new_node->val = value;
    new_node->next = NULL;

   if(dll->len == 0) {
        new_node->prev = NULL;
        dll->head = new_node;
        dll->tail = new_node;
    }
   else {
        new_node->prev = dll->tail;
        dll->tail->next = new_node;
        dll->tail = new_node;
    }
    
    dll->len++;
}

void DLL_insertFront(DoublyLinkedList* dll, int value) {
    Node* new_node = (Node*)dll->allocator.alloc(sizeof(Node), dll->allocator.context); 
    if(new_node == NULL) {
        fprintf(stderr, "Fatal: ran out of memory.\n");
        return;
    }

    new_node->val = value;
    new_node->prev = NULL;

   if(dll->len == 0) {
        new_node->next = NULL;
        dll->head = new_node;
        dll->tail = new_node;
    }
   else {
        new_node->next = dll->head;
        dll->head->prev = new_node;
        dll->head = new_node;
    }
    
    dll->len++;
}
void DLL_print(DoublyLinkedList* dll) {
    Node* current_node = dll->head;
    printf("NULL <- ");
    for(size_t i=0; i<dll->len-1; i++) {
        printf("%d <-> ", current_node->val);
        current_node = current_node->next;
    }
    
    printf("%d -> NULL\n", current_node->val);
    current_node = current_node->next;
}
