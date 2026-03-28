#include <iostream>
#include "areana.h"
#include <chrono>
#include <vector>
struct MyData {
    int id;
    double value;
};
const int num_allocations = 100000;
int main() {
    Areana a(num_allocations * sizeof(MyData) * 2, alignof(MyData));
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_allocations; i++) {
        a.allocate(sizeof(MyData));
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "Arena Allocator: " << duration.count() << " microseconds\n";
    std::vector<MyData*> new_ptrs;
    new_ptrs.reserve(num_allocations);
    auto start_new = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < num_allocations; i++) {
        new_ptrs.push_back(new MyData());
    }
    auto end_new = std::chrono::high_resolution_clock::now();
    auto duration_new = std::chrono::duration_cast<std::chrono::microseconds>(end_new - start_new);
    std::cout << "cpp new: " << duration_new.count() << " microseconds\n";
    for (MyData* ptr : new_ptrs) delete ptr;
    return 0;
}
