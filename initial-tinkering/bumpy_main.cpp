#include "bump_allocator.h"
#include <iostream>
#include <chrono>
#include <vector>
struct MyData{
    int id;
    double value;
};
int main (){
    const int num_allocation=1000000;
    BumpAllocator bump_allocator(sizeof(MyData)*num_allocation+1024);
    auto start_time_bump=std::chrono::high_resolution_clock::now();
    for (int i=0;i<num_allocation;i++){
        bump_allocator.allocate(sizeof(MyData));
    }
    auto end_time_bump=std::chrono::high_resolution_clock::now();
    auto duration_bump=std::chrono::duration_cast<std::chrono::microseconds>(end_time_bump-start_time_bump);
    std::cout<<"Bump Allocator: " <<duration_bump.count() << " microseconds" << std::endl;

    std::vector<MyData*> new_pointers;
    new_pointers.reserve(num_allocation);
    auto start_time_new=std::chrono::high_resolution_clock::now();
    for (int i=0;i<num_allocation;i++){
        new_pointers.push_back(new MyData());
    }
    auto end_time_new=std::chrono::high_resolution_clock::now();
    auto duration_new=std::chrono::duration_cast<std::chrono::microseconds>(end_time_new-start_time_new);
    std::cout << "Standard 'new' : " << duration_new.count() << " microseconds" << std::endl;
    for (MyData* ptr:new_pointers){
        delete ptr;
    }
return 0;
}
