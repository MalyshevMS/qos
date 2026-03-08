#include "heap.hpp"
#include <stdint.h>

#define HEAP_START (0x200000)
#define HEAP_SIZE  (0x100000)

struct BlockHeader {
    size_t size;
    bool free;
    BlockHeader* next;
};


static BlockHeader* heap_head = nullptr;

void Mem::init() {
    heap_head = (BlockHeader*)HEAP_START;

    heap_head->size = HEAP_SIZE - sizeof(BlockHeader);
    heap_head->free = true;
    heap_head->next = nullptr;
}

void *Mem::malloc(size_t size) {
    size = (size + 7) & ~7;
    auto current = heap_head;

    while (current) {
        if (current->free && current->size >= size) {
            if (current->size > size + sizeof(BlockHeader)) {
                auto new_block = (BlockHeader*)((uint8_t*)current + sizeof(BlockHeader) + size);
    
                new_block->size = current->size - size - sizeof(BlockHeader);
                new_block->free = true;
                new_block->next = current->next;
    
                current->next = new_block;
                current->size = size;
            }

            current->free = false;

            return (uint8_t*)current + sizeof(BlockHeader);
        }

        current = current->next;
    }

    return nullptr;
}

void Mem::free(void *ptr) {
    if (!ptr) return;

    auto block = (BlockHeader*)((uint8_t*)ptr - sizeof(BlockHeader));

    block->free = true;

    auto current = heap_head;

    while (current) {
        if (current->free && current->next && current->next->free) {
            current->size += sizeof(BlockHeader) + current->next->size;

            current->next = current->next->next;
        }

        current = current->next;
    }
    
}
