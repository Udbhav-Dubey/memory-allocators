#include <cstdlib>
#include <iostream>
#include <cassert>
class Pool{
    private:
        size_t total_space;
        size_t slots;
        size_t fss;// fixed slot size;
       char* head{nullptr};
       char*base{nullptr};
    public:
        Pool(size_t ts,size_t s):total_space{ts},slots{s}{
            if (s>ts){      std::cout << "hey not allowed \n will use assert here rn not sure of syntax of assert\n";    
           }
           fss=ts/s; 
            assert(fss>=sizeof(void*));
           base=(char*)malloc(sizeof(char)*ts);
        head=base;
            size_t i=0;
           char* curr{nullptr};
            for(size_t i=0;i<slots;i++){
                curr=head+(i*fss);
            *(char**)curr=base+((i+1)*fss);
          //  *(char**)curr=head;
            i++;
        
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
    std::  cin>>ts>>s;
    Pool pool(ts,s);
    std:: cin>>x;
    char*ptr_x=pool.allocate(x);
    pool.pfree(ptr_x);
    return 0;
}
