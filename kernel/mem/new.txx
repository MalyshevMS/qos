#pragma once

#include "heap.hpp"

void* operator new(size_t size) {
    return Mem::malloc(size);
}

void operator delete(void* ptr) {
    Mem::free(ptr);
}

void* operator new[](size_t size) {
    return Mem::malloc(size);
}

void operator delete[](void* ptr) {
    Mem::free(ptr);
}