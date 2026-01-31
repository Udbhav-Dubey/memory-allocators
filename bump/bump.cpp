#include <iostream>
#include "bump.h"
#include <cstdint>
bump::bump(void* buffer,std::size_t size_ask) noexcept{
    curr=static_cast<char*>(buffer);
    end=static_cast<char*>(buffer)+size_ask;
    beg=curr;
}
void* bump::allocate(std::size_t ask,std::size_t alignment) noexcept{
    if ((alignment&(alignment-1))){std::cout << "only alignment of multiple of  2 possible\n";
    return nullptr;
    }
    char *oldcurr{nullptr};
    uintptr_t mask=alignment-1;
    uintptr_t addr=reinterpret_cast<uintptr_t>(curr);
    addr=(addr+mask)&~mask;
    char* cu=reinterpret_cast<char*>(addr);
    if (cu>end){//std::cout <<"cannot allocate this much memory allignment issue\nreturning nullptr for now\n";
    return nullptr;
    }
    if (cu+ask>end){//std::cout <<"cannot allocate as ask size is too much big try lower value\nreturning nullptr for now\n";
    return nullptr;
    }
    curr=cu;
    oldcurr=curr;
    curr+=ask;
    return oldcurr;
}
void bump::reset()noexcept{
    curr=beg;
}
std::size_t bump::capacity() const noexcept {
    return end-beg;    
}
std::size_t bump::used()const noexcept{
    return curr-beg;
}
std::size_t bump::remaining()const noexcept{
    return end-curr;
}

