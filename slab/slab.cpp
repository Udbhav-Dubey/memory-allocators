#include "slab.h"
slab::slab(size_t t,size_t ts):block_size{t},total_size{ts}{
    slots=total_size/block_size;
    assert(ts>=t&&"Total space should >size defined for type\n");
    assert(slots>0&&"slots must be greater than zero\n");
    base=(char*)malloc(sizeof(char)*ts);
    max_size=base+total_size;
    assert(base!=nullptr);
    head=base;
    char*curr{nullptr};
    for (size_t i=0;i<slots;i++){
        curr=base+(i*block_size);
        if (i==slots-1){
            *(char**)curr=nullptr;
        }
        else {
            *(char**)curr=base+(i+1)*block_size;
        }
    }
}
char* slab::allocate(){
    if (used_count>=slots){
        assert("used_count>=slots so failure in allocation\n");
    }
    if (head==nullptr){return nullptr;}
    char*temp=head;
    head=*(char**)temp;
    used_count++;
    return temp;
}
void slab::pfree(char*ptr_x){
    assert(ptr_x>=base&&ptr_x<max_size);
    assert((ptr_x-base)%block_size==0);
    *(char**)ptr_x=head;
    head=ptr_x;
    used_count--;
}
bool slab::full(){
    return used_count>=slots;
}
bool slab::partial(){
    return used_count<slots && used_count!=0;
}
bool slab::empty(){
    return used_count==0;
}
void slab:: print_debug(){
    char*curr=head;
    std::cout << "slots : "<< slots << "\n";
    std::cout << "used_count : " << used_count <<"\n";
    for (size_t i=0;i<slots&&curr!=nullptr;i++){
        std::cout << (void*)curr<<"-> ";
        curr=*(char**)curr;
    }
    std::cout << "nullptr\n";
}
bool slab:: checkrange(char*x){
    if (x>=base&&x<max_size){
        return 1;
    }
    return 0;
}
slab::~slab(){
    free(base);
}
