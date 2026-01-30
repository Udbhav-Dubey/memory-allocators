#pragma once
#include <cstddef>
class bump{
private:
    char* curr{nullptr};
    char* end{nullptr};
    char* beg{nullptr};
public:
    bump(void* buffer,std::size_t size) noexcept;
    void* allocate(std::size_t ask,std::size_t alignment) noexcept;
    void reset() noexcept;
    std::size_t capacity() const noexcept;
    std::size_t used() const noexcept; 
    std::size_t remaining() const noexcept;
    
    bump(const bump&)=delete;
    bump& operator=(const bump&)=delete;
};
