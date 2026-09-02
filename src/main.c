#include <stdio.h>
#include "inc/doubly_liked_list.h"

int main() {
    DoublyLinkedList* dll = DLL_create(DEFAULT_ALLOCATOR);
    
    DLL_insertEnd(dll, 1);
    DLL_print(dll);
    DLL_insertEnd(dll, 2);
    DLL_insertEnd(dll, 3);
    DLL_insertEnd(dll, 4);
    DLL_insertEnd(dll, 5);
    DLL_print(dll);
    DLL_insertFront(dll, -67);
    DLL_print(dll);
}
