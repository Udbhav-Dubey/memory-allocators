#pragma once
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cstddef>
class Pool{
    private:
        size_t total_space;
        size_t slots;
        size_t fixed_size_slots;
        char *head;
        char *base;
        char *max_size;
    public:
        Pool(size_t ts,size_t s);
        char *allocate(size_t x);
        void* sendError();
        void pfree(char *ptr_x);
        void print_debug();
        ~Pool();
};
