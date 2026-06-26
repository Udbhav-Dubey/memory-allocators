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
void cache::debug_type(std::vector<slab*>x){
    if (x.size()==0){std::cout << "nothing in here\n";}
    else {
        for (auto s:x){
            s->print_debug();
        }
    }
}
void cache::debug(){
    std::cout <<"used\n";
    debug_type(used);
    std::cout << "empty\n";
    debug_type(empty);
    std::cout <<"partial\n";
    debug_type(partial);
    std::cout <<"\n";
}
cache::cache(size_t ts,size_t t){
    tsize=ts;
    total_size=t;
}
char* cache::allocate(){
    char*x=nullptr;
    slab*temp=nullptr;
    if (partial.size()==0){
        if (empty.size()==0){
        temp=new slab(tsize,total_size);
        }
    else{
        temp=empty.back();
        empty.pop_back();
    }
    }
    else {
       temp=partial.back();
       partial.pop_back();
    }
    x=temp->allocate();
    if (temp->full()){
        used.push_back(temp);
    }
    else {
        partial.push_back(temp);
    }
    return x;
}
void cache::cfree(char*x){
    bool found=false;
    for (auto &s:partial){
        if (s->checkrange(x)){
            s->pfree(x);
            if (s->empty()){
                empty.push_back(s);
                std::swap(s,partial.back());
                partial.pop_back();
            }
            found=1;
            break;
        }
    }
    if (found==1){return ;}
    for (auto &s:used){
        if (s->checkrange(x)){
            s->pfree(x);
            if (s->empty()){
                empty.push_back(s);
            }
            else {
                partial.push_back(s);
            }
            std::swap(s,used.back());
            used.pop_back();
            found=1;
            break;
        }
    }
    assert(found==true&&"not found here\n");
}
cache::~cache(){
    for (auto s:empty)delete s;
    for (auto s:partial)delete s;
    for (auto s:used)delete s;
}
