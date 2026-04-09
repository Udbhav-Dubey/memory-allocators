#include <cstdlib>
#include <iostream>
#include <cassert>
#include <algorithm>
#include <cstddef>
size_t align_up(size_t n,size_t a){
    return (n+a-1)&~(a-1);
}
class Pool{
    private:
        size_t total_space;
        size_t slots;
        size_t fss;// fixed slot size;
       char* head{nullptr};
       char*base{nullptr};
       char* max_size{};
    public:
        Pool(size_t ts,size_t s):total_space{ts},slots{s}{
        //    if (s>ts){      std::cout << "hey not allowed \n will use assert here rn not sure of syntax of assert\n";    
         //  }
         assert(s>0);
         assert(ts>=s);
           fss=ts/s; 
           fss=std::max(fss,sizeof(void*));
           size_t alignment=alignof(std::max_align_t);
            fss=align_up(fss,alignment);
            assert(fss>=sizeof(void*));
            slots=ts/fss;
            total_space=slots*fss;
            assert(slots>0);
            // malloc here bitch , get the base and final 
           base=(char*)malloc(sizeof(char)*ts);
           max_size=base+total_space;
           assert(base!=nullptr);
        head=base;
           char* curr{nullptr};
            for(size_t i=0;i<slots;i++){
                curr=base+(i*fss);
       //     *(char**)curr=base+((i+1)*fss);
          //  *(char**)curr=head;
        
            if (i==slots-1){
                *(char**)curr=nullptr;
                }
            else {
            *(char**)curr=base+(i+1)*fss;
            }
            }
    }
        char *allocate(size_t x){
            if (head==nullptr){return nullptr;}
            if (x>fss){
                std::cout << "hey not allowed \n will use assert here rn not sure of syntax of assert\n";    
                return nullptr;
            }
            char *temp=head;
            head=*(char**)temp;
            return temp;
        }
        void pfree(char*ptr_x){
            if (ptr_x<base||ptr_x>max_size){std::cout << "not my pool mate, get out\n";}
            *(char**)ptr_x=head;
            head=ptr_x;
        }
    ~Pool(){
        free(base);
    }
};
int main (){
    // for now lets consider user gives space and slots also 
    // becuase if they onyl give total space then whats the best way to make slots i dont know alright so for now lets let them select slot 
    size_t ts,s,x;
    std::cout << "Total size \n then slots \n";
    std::  cin>>ts>>s;
    Pool pool(ts,s);
    std::cout << "enter allocation size:\n";
    std:: cin>>x;
    char*ptr_x=pool.allocate(x);
    char c='a';
   char *px=&c;
    pool.pfree(px);
    // lets check double free now 
    pool.pfree(ptr_x);
//    pool.pfree(ptr_x);
    return 0;
}
