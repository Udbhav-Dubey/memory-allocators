#include <bits/stdc++.h>
using namespace std;
class Pool{
    private:
        size_t total_space;
        size_t slots;
        size_t fss;// fixed slot size;
       // size_t* tsp; // total space ptr;
       char* head{nullptr};
    public:
        Pool(size_t ts,size_t s):total_space{ts},slots{s}{
           fss=ts/s; 
          // tsp=(char*)malloc(sizeof(char)*ts);
            int i=0;
           char* curr{nullptr};
            while (i*fss<total_space){
            curr=(char*)malloc(sizeof(char)*fss);
            *(char**)curr=head;
            head=curr;
            i++;
        }
    }   
};
int main (){
    // for now lets consider user gives space and slots also 
    // becuase if they onyl give total space then whats the best way to make slots i dont know alright so for now lets let them select slot 
    size_t ts,s;
    cin>>ts>>s;
    Pool pool(ts,s);

    return 0;
}
