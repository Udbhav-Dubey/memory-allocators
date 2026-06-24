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
//partial empty used fixes sizes or on demand work ?
// vector for these guys idk need to control them partial has half complete slabs , used has completed slabs, empty has empy do what -> intially put slabs in empty only then start taking from it only;empty size? 
// what if a linked list such that it solves the whole size problem everything on demand since its same type nodes can handle themselves , linked list, but thats extra metadata same as vector idk 
class cache{
    int cache_type{}; // 64 128 bytes vagera 
    vector<slab*>partial;
    vector<slab*>empty;
    vector<slab*>used;
    public:
    cache(int ct,int es,int total_size){
        cache_type=ct;
        int s_size=total_size/es;
        for (int i=0;i<es;i++){
            slab& temp= &slab t(ct,s_size); // i know this is wrong but for few minutes assume its rights and write other part thinking each have slab*
            used.push_back(temp);
        }    
    }
    int get_cache_type{
        return cache_type;
    };
    char *alloc(int size){
        char*x=nullptr;
 //      for (auto s:empty){
/*            if (s.empty()==1){// allocation can happen
     }
            if (s.partial()==1){//allocation can happen but this should be moved to partial since all allocations happen from there 
    }
            if (s.full()==1){//move to full 
    }*/
            if (partial.size()==0){
                for (auto s:empty){
                    // but why does an empty has a partial 
                    x=s->allocate(size);
                    if (s->partial()==1){
                    // shouldnt this slab live in partial though
                    //can i even push like this maybe i should take slab pointers so its easy to send fuck a linked list could be better but you get the idea 
                    //but how does it get removed from here empty ? 
                    partial.push_back(s);
                    }
                    else if (s->full()==1){
                        full.push_back(s);
                    }
                }
            }
            else { 
                for (auto s:partial){
                    x=s->allocate(size);
                    if (s->full()==1){
                        full.push_back(s);
                    }
                }
            }
//}      
        return x;
    }
    void free(char*x){
        for (auto s:partial){
            s->pfree(x);
            if (s->empty()==1){
                empty.emplace_back(s);
            }
        }
    }
};
class slabAllocator{
    
};
int main (){
    // currently rely on allignment since slab will not deal with any of that allignment would be dealt by slabAllocator class ;
 /*   slab s(64,128);
    s.print_debug();
    char*x=s.allocate(55); 
    s.print_debug();
    char*y=s.allocate(32);
    s.print_debug();
    char*z=s.allocate(34);
    s.pfree(x);
    s.print_debug();
    s.pfree(y); 
    */
    //this was just slab part no need to focus on cache part
    
    
return 0;
}
