// slab->pool allocator with metadata
// cache->manager of slab of one class
// slabAllocator->public api that chooses a cache
#include <iostream>
#include <cassert>
#include <cstdlib>
class slab{
    int size_type{};
    int total_size{};
    int slots{};
    char*base{nullptr};
    char*head{nullptr};
    char*max_size;
    int used_count{};
    public:
        slab(int t,int ts):size_type{t},total_size{ts} {
        slots=total_size/size_type;
        assert(ts>=t&&"total space should be greater than type\n");
        assert(slots>0&&"slots must be greater than zero\n");
        base=(char*)malloc(sizeof(char)*ts);
        max_size=base+total_size;
        assert(base!=nullptr);
        head=base;
        char*curr{nullptr};
        for (size_t i=0;i<slots;i++){
            curr=base+(i*size_type);
            if (i==slots-1){
                *(char**)curr=nullptr;
            }
            else {
                *(char**)curr=base+(i+1)*size_type;
            }

        }
        }
        char*allocate(int x){
            if (used_count>slots){
                return nullptr;
            }
            if (head==nullptr){return nullptr;}
            assert(x<=size_type&&"cannot allocate more than size go for diffrent size\n");
            char*temp=head;
            head=*(char**)temp;
            used_count++;
            return temp;
        }
        void pfree(char*ptr_x){
            assert(ptr_x>=base&&ptr_x<max_size);
            assert((ptr_x-base)%size_type==0);
            *(char**)ptr_x=head;
            head=ptr_x;
            used_count--;
        }
        bool full(){
            return used_count>=slots;
        }
        bool partial(){
            return used_count<slots && used_count!=0;
        }
        bool empty(){
            return used_count==0;
        }
        void print_debug(){
 //           if (!head){return ;}
            char*curr=head;
            std::cout << "slots : "<< slots << "\n";
            std::cout << "used_count : "<< used_count<<"\n";
            for (int i=0;i<slots&&curr!=nullptr;i++){
                std::cout << (void*)curr<<"-> ";
                curr=*(char**)curr;
            }
            std::cout <<"nullptr\n";
        }
};
class cache{
    
};
class slabAllocator{
    
};
int main (){
    // currently rely on allignment since slab will not deal with any of that allignment would be dealt by slabAllocator class ;
    slab s(64,128);
    s.print_debug();
    char*x=s.allocate(55); 
    s.print_debug();
    char*y=s.allocate(32);
    s.print_debug();
    char*z=s.allocate(34);
    s.pfree(x);
    s.print_debug();
    s.pfree(y);
return 0;
}
