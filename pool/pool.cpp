#include "pool.h"
size_t alignup(size_t n,size_t a){
    return (n+a-1)&(~(a-1));
}
Pool::Pool(size_t ts,size_t s):total_space{ts},slots{s} {
    assert(s>0);
    assert(ts>=s);
    fixed_size_slots=total_space/slots;
    fixed_size_slots=std::max(fixed_size_slots,sizeof(void*));
    size_t allignment=alignof(std::max_align_t);
    fixed_size_slots=alignup(fixed_size_slots,allignment);
    assert(fixed_size_slots>=sizeof(void*));
    slots=ts/fixed_size_slots;
    total_space=slots*fixed_size_slots; // vapis taki alligned up hojae;
    assert(slots>0);
    base=(char*)malloc(sizeof(char)*total_space);
    if (base==nullptr){assert(base!=nullptr);return ;}
    max_size=base+total_space;
    head=base;
    char *curr{nullptr};
    for (size_t i=0;i<slots;i++){
        curr=base+(i*fixed_size_slots);
        if (i==slots-1){
            *(char**)curr=nullptr;
        }
        else {
            *(char**)curr=base+(i+1)*fixed_size_slots;
        }
    }
}
char* Pool::allocate(size_t x){
    if (head==nullptr){return nullptr;}
    if (x>fixed_size_slots){
        assert(x<=fixed_size_slots&&"given size should be lesser than slot size \n");
        return nullptr;
    }
    char*temp=head;
    head=*(char**)temp;
    return temp;
}
void* Pool::sendError(){
    return nullptr;
}
void Pool::pfree(char* ptr_x){
    assert(ptr_x>=base&&ptr_x<max_size);
    assert((ptr_x-base)%fixed_size_slots==0);
    *(char**)ptr_x=head;
    head=ptr_x;
}
void Pool::print_debug(){
    if (!head){return ;}
    char*curr=head;
    for (int i=0;i<slots&&curr!=nullptr;i++){
        std::cout << (void*)curr << "-> ";
        curr=*(char**)curr;
    }
    std:: cout << "nullptr\n";
}
Pool::~Pool(){
    free(base);
}
