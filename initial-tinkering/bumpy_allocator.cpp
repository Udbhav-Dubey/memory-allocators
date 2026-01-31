#include "bump_allocator.h"
#include <iostream>
#include <new> 
BumpAllocator::BumpAllocator(size_t buffer_size)
    :m_buffer_size(buffer_size),m_offset(0){
        m_buffer=new(std::nothrow) char[m_buffer_size];
        if (!m_buffer){
            std::cerr<<"Failed to allocate  memory for the buffer ." << std::endl;
        }
    }
BumpAllocator::~BumpAllocator(){
    delete[] m_buffer;
}
void *BumpAllocator::allocate(size_t size){
    if (!m_buffer){
        return nullptr;
    }    
    size_t alligned_offset=(m_offset+7)&~7;
    if (alligned_offset+size>m_buffer_size){
        std::cerr<< "Error:Out of memory in bump allocator." << std::endl;
        return nullptr;
    }
    void *result=m_buffer+alligned_offset;
    m_offset=alligned_offset+size;
    return result;
}
void BumpAllocator::reset(){
    m_offset=0;
}
