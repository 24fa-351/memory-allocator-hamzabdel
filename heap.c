#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "heap.h"

#define HEAP_SIZE 1024 * 1024

typedef struct Block {
    size_t size;
    int free;
    struct Block* next;
} Block;

static void* heap_start = NULL;
static Block* free_list = NULL;

void* get_me_blocks(ssize_t how_much)
{
    void* ptr = sbrk(0);

    if (sbrk(how_much) == (void*)-1) {
        return NULL;
    }
    return ptr;
}

void* init_heap()
{
    if (heap_start == NULL) {
        heap_start = get_me_blocks(HEAP_SIZE);
        if (heap_start == NULL) {
            return NULL;
        }
        free_list = (Block*)heap_start;
        free_list->size = HEAP_SIZE - sizeof(Block);
        free_list->free = 1;
        free_list->next = NULL;
    }
    return heap_start;
}

void* malloc(size_t size)
{
    init_heap();

    Block* current = free_list;
    while (current != NULL) {
        if (current->free && current->size >= size) {
            if (current->size > size + sizeof(Block)) {
                Block* new_block
                    = (Block*)((char*)current + sizeof(Block) + size);
                new_block->size = current->size - size - sizeof(Block);
                new_block->free = 1;
                new_block->next = current->next;
                current->size = size;
                current->free = 0;
                current->next = new_block;
            } else {
                current->free = 0;
            }
            return (char*)current + sizeof(Block);
        }
        current = current->next;
    }

    Block* new_block = get_me_blocks(size + sizeof(Block));
    if (new_block == NULL) {
        return NULL;
    }
    new_block->size = size;
    new_block->free = 0;
    new_block->next = NULL;

    if (free_list == NULL) {
        free_list = new_block;
    }
    return (char*)new_block + sizeof(Block);
}

void free(void* ptr)
{
    if (ptr == NULL) {
        return;
    }

    Block* block = (Block*)((char*)ptr - sizeof(Block));
    block->free = 1;

    Block* current = free_list;
    while (current != NULL) {
        if (current->free && current->next && current->next->free) {
            current->size += sizeof(Block) + current->next->size;
            current->next = current->next->next;
        }
        current = current->next;
    }
}
