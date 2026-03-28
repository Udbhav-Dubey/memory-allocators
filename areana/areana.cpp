#include "areana.h"
#include <cstdlib>
#include <cassert>
Chunk::Chunk(size_t N){
    arr=(char*)malloc(sizeof(char)*N);
    start=arr;
    end=arr+N;
    curr=arr;
}
char* Chunk::alloc(size_t N,size_t allign){
assert(allign!=0&&(allign&(allign-1))==0);
        uintptr_t mask=allign-1;
        uintptr_t ucur=reinterpret_cast<uintptr_t>(curr);
        ucur=(ucur+mask) & ~(mask);
        char* check=reinterpret_cast<char*>(ucur);
        if (check+N>end){
            return nullptr;
        }
        curr=check;
        char*oldcurr(curr);
        curr+=N;
        return oldcurr;
    }
Chunk::Chunk(Chunk&&x)noexcept{
        arr=x.arr;
        x.arr=nullptr;
        start=x.start;
        x.start=nullptr;
        curr=x.curr;
        x.curr=nullptr;
        end=x.end;
        x.end=nullptr;
    }
Chunk&Chunk::operator=(Chunk&&x)noexcept{
        if (this==&x)return *this;
        free(arr);
        arr=x.arr;
        x.arr=nullptr;
        curr=x.curr;
        x.curr=nullptr;
        start=x.start;
        x.start=nullptr;
        end=x.end;
        x.end=nullptr;
        return *this;
    }
void Chunk::reset(){
    curr=start;
}
Chunk::~Chunk(){
    free(arr);
}
Areana::Areana(size_t n,size_t a){
    areana.emplace_back(n);
    ali=a;
    extra=n;
}
char* Areana::allocate(size_t byt){
    char* temp=areana.back().alloc(byt,ali);
            while(!temp){
                areana.emplace_back(byt+extra);
                temp=areana.back().alloc(byt,ali);
            }
            return temp;
}
Areana::~Areana(){
    areana.clear();
}
