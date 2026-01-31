#include <iostream>
#include "bump.h" 
#include <chrono>
#include <thread>
#include <vector>
struct Mydata{
    int id;
    double value;
};
const int num_allocation=100000;
const size_t buffer_size=num_allocation*sizeof(Mydata)*2;
int main (){
    char* buf=new char[buffer_size];
    bump b(buf,buffer_size);
    auto start=std::chrono::high_resolution_clock::now();
    for (int i=0;i<num_allocation;i++){
        b.allocate(sizeof(Mydata),8);
    }
    auto end=std::chrono::high_resolution_clock::now();
    auto duration=std::chrono::duration_cast<std::chrono::microseconds>(end-start);
    std::cout <<"Bump Allocator: " << duration.count() << "microseconds" << std::endl;
    std::vector<Mydata*>new_ptrs;
    new_ptrs.reserve(num_allocation);
    auto start_time_new=std::chrono::high_resolution_clock::now();
    for (int i=0;i<num_allocation;i++){
        new_ptrs.push_back(new Mydata());
    }
    auto end_time_new=std::chrono::high_resolution_clock::now();
    auto duration_new=std::chrono::duration_cast<std::chrono::microseconds>(end_time_new-start_time_new);
    std::cout<<"cpp new : " << duration_new.count() << "microseconds" << std::endl;
    delete[] buf;
    for (Mydata*ptr:new_ptrs){
        delete ptr;
    }
    return 0;
}
