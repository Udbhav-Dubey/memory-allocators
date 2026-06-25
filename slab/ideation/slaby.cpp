// slab->pool allocator with metadata
// cache->manager of slab of one class
// slabAllocator->public api that chooses a cache
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <utility>
class slab{
    int size_type{};
    int total_size{};
    int slots{};
    char*base{nullptr};
    char*head{nullptr};
    char*max_size;
    int used_count{};
    public:
    slab(slab&)=delete;
    slab operator=(slab&)=delete;
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
            if (used_count>=slots){
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
        bool checkrange(char*x){
            if (x>=base&&x<max_size){
                return 1;
            }
            return 0;
        }
        ~slab(){
            free(base);
        }
};
//partial empty used fixes sizes or on demand work ?
// vector for these guys idk need to control them partial has half complete slabs , used has completed slabs, empty has  do what -> intially put slabs in empty only then start taking from it only;empty size? 
// what if a linked list such that it solves the whole size problem everything on demand since its same type nodes can handle themselves , linked list, but thats extra metadata same as vector idk 
class cache{
    std::vector<slab*>used;
    std::vector<slab*>empty;
    std::vector<slab*>partial;
   int tsize{};
   int total_size{};
   void debug_type(std::vector<slab*>x){
       if (x.size()==0){std::cout << "nothing in "<< "here" <<"\n";}
        else {
            for (auto s:x){
                s->print_debug();
            }
        }
   }
    public:
   cache(cache&)=delete;
   cache operator=(cache&)=delete;
   void debug(){
       std::cout << "used\n"; 
       debug_type(used);
       std::cout<<"empty\n";
        debug_type(empty);
        std::cout << "partial\n";
        debug_type(partial);
        std::cout << "\n";
   }
   cache(int ts,int t){
        tsize=ts;
        total_size=t;
   }
   char*allocate(){
       char*x=nullptr;
       // agar partial 0 to empty se nikalo nahi to partial pe hi kaam karo 
        if (partial.size()==0){
            if (empty.size()==0){
               // new slab required 
               slab*temp=new slab(tsize,total_size);
                x=temp->allocate(tsize);
                if (temp->full()){
                    used.push_back(temp);
                }
                else {
                partial.push_back(temp);
                }
 //               std::cout <<"is it even pushed printing partial size : " << partial.size() << "\n";
            }
            else {
                // but how does empty get things ? i am directy sending partial will fix this for now just
                // take temp from empty but how // take last one and pop_back?
                slab*temp=empty.back();
                empty.pop_back();
                x=temp->allocate(tsize);
                if (temp->full()){
                used.push_back(temp);
                }
                else {
                    partial.push_back(temp);
                }
   //             std::cout <<"is it even pushed printing partial size : " << partial.size() << "\n";
            
            }
        }
        // partial se  pe hai lelo slab wahain se lelo
        else {
          slab*temp=partial.back();
          partial.pop_back();
          x=temp->allocate(tsize);
          if (temp->full()==1){
            used.push_back(temp);
          }
          else {
            partial.push_back(temp);
          }
             //  std::cout <<"is it even pushed printing partial size : " << partial.size() << "\n";
  
        }
   return x;
   }
   // does a block goes back to same slab , i think so but if all the slabs are of same size and does same thing for one cahce does changing good or not ??? 
   // lets assume yes same slab so for that what i could do is take a block and check range the range it is in we will simply insert but this way would be o(n) because i would need to search in partial and used the ranges and call free of slab
   // so create checkrange in slab also to check ;
   void cfree(char*x){
       bool found=false;
//            if (partial.size()==0){std::cerr<<"cannot free since no block allocated\n";}       
            for (auto &s:partial){
                if (s->checkrange(x)){
                    s->pfree(x);
                    // need to remove s from here as well should i make it point to nullptr? swap with back and pop;
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
                // need to remove s from here as well should i make it point to nullptr?rebuild it ?;
                    if (s->empty()){
                        empty.push_back(s);
                    }
                    else{
                        partial.push_back(s);
                    }
                    std::swap(s,used.back());
                    used.pop_back();
                    found=1;
                    break;
                
                }
            }
       assert(found==true&&"not here mate\n");
   }
   ~cache(){
    for(auto s : empty) delete s;
    for(auto s : partial) delete s;
    for(auto s : used) delete s;
}
};
class slabAllocator{
    private:
        int totalsize{};
       // vector<cache*>cach;
        std::unordered_map<int,cache*>mp;
        std::unordered_map<char*,cache*>ptc;
        int findsize(int n){
            if (n<0){return -1;}
            if (n==0){return 1;}
            n--;
            n|=n>>1;
            n|=n>>2;
            n|=n>>4;
            n|=n>>8;
            n|=n>>16;
            //n|=n>>32;
            return n+1;
        }
    public:
        // should someone decide total size per cache or off the whole thing if the whole thing how do we divide for each slab , there is no way for slab to resize their total size , either we reserve or idk tbh 
        slabAllocator(int ts){
            totalsize=ts;
        }
        char*allocate(int x){
            assert(x<totalsize&&"c'mon dawg you selected the size in first place\n");
            int n=findsize(x);
            if (mp.find(n)==mp.end()){
               cache*temp=new cache(n,totalsize);
               mp[n]=temp; 
            }
            char *ptr_x=mp[n]->allocate();
            assert(ptr_x!=nullptr&&"something went wrong in allocation after accessing from map\n");
            ptc[ptr_x]=mp[n];
            return ptr_x;
        }
        void free(char*x){
            assert(ptc.find(x)!=ptc.end()&&"double free on same\n");
           // detemine size with help of ptr no direct way in my opinion need to loop , if there is a direct way i would need to store reverse hashmap also else by just address its not possible to find size; 
           ptc[x]->cfree(x);
           ptc.erase(x);
        }
        ~slabAllocator(){
            for (auto &[k,v]:mp){
                delete v;
            }
                mp.clear();
                ptc.clear();

        }
        void debug(){
            for (auto &[s,c]:mp){
                std::cout << "cache  : " << s << "\n";
                c->debug();
            }
        }
};
int main (){
   /* int x=9;
    x=~(x&0);
    std::cout << x+1 << "\n";
    */
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
   
    //this was just slab part no need to focus on cache part
   cache ch(64,192); 
   ch.debug();
    char*vv=ch.allocate();  
    ch.debug();
    char*aa=ch.allocate();  
    ch.debug();
    char*xx=ch.allocate();  
    ch.debug();
     char*zz=ch.allocate();  
    ch.debug();
    ch.cfree(vv);
    ch.debug();
     
    cache ch1(64,192);

    std::cout << "initial\n";
    ch1.debug();

    char* a1 = ch1.allocate();
    char* b1 = ch1.allocate();
    char* c1 = ch1.allocate();

    std::cout << "after 3 allocs\n";
    ch1.debug();

    char* d1 = ch1.allocate();

    std::cout << "after 4th alloc\n";
    ch1.debug();

    ch1.cfree(a1);

    std::cout << "free(a)\n";
    ch1.debug();

    ch1.cfree(b1);

    std::cout << "free(b) \n";
    ch1.debug();

    ch1.cfree(c1);

    std::cout << "free(c)\n";
    ch1.debug();

    char* ee = ch1.allocate();

    std::cout << "alloc(e) \n";
    ch1.debug();
    char* ptrs[20];

for(int i=0;i<20;i++)
    ptrs[i]=ch1.allocate();

for(int i=0;i<20;i++)
    ch1.cfree(ptrs[i]);

ch1.debug();

    cache ch2(64,192);

char* ptrs1[20];

for(int i=0;i<20;i++)
    ptrs1[i]=ch2.allocate();

for(int i=0;i<20;i++)
    ch2.cfree(ptrs1[i]);

ch2.debug();
    slabAllocator alloc(192);
    alloc.debug();
    char* aaa = alloc.allocate(50);
    char* bbb = alloc.allocate(60);
    char* ccc = alloc.allocate(63);
    alloc.debug();
    char* ddd = alloc.allocate(64);
    alloc.debug();
    char* eee = alloc.allocate(70);
    alloc.debug();
    char* fff = alloc.allocate(20);
    alloc.debug();
    alloc.free(aaa);
    alloc.free(eee);
    alloc.free(fff);
    alloc.debug();
    char* ggg = alloc.allocate(18);
    char* hhh = alloc.allocate(100);
    alloc.debug();
    alloc.free(bbb);
    alloc.free(ccc);
    alloc.free(ddd);
    alloc.free(ggg);
    alloc.free(hhh);
    alloc.debug();
    //s.pfree(z);
    //ch.cfree(aa);
//ch.cfree(xx);
//ch.cfree(zz);
//ch1.cfree(d1);
//ch1.cfree(ee);
return 0;
}
