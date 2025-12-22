#ifndef BUMP_ALLOCATOR_H
#define BUMP_ALLOCATOR_H
#include <cstdint>
#include <iostream>
class BumpAllocator{
    public : 
        BumpAllocator(size_t buffer_size);
        ~BumpAllocator();
        void* allocate(size_t size);
        void reset();
    private:
        char* m_buffer;
        size_t m_buffer_size;
        size_t m_offset;
};
#endif 
