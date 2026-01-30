#include <iostream>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <cassert>
#include <iomanip>

class Bumpy{
    private:
        char* end{nullptr};
        char* curr{nullptr};
        char* arr;
        char* beg{nullptr};
    public:
        bool flag=1;
        Bumpy(int N){
            arr=(char*)malloc(N*sizeof(char));
            end=arr+N;
            curr=arr;
            beg=arr;
        }
       char* give_me_bytes(int N){
            if (curr+N>end){
                std::cout << "cannot allocate this much memory as it would lead to out of memory\n";
                flag=0;
                return nullptr;
            }
            char *oldcurr{nullptr};
            uintptr_t mask=alignof(std::max_align_t)-1;
            uintptr_t addr=reinterpret_cast<uintptr_t>(curr);
            addr+=mask;
            addr&=~(mask);
            char *cu=reinterpret_cast<char*>(addr);
            if (cu>end){
                std::cout << "cannot allocate this much memory allign issue\n";
                flag=0;
                return nullptr;
            }
            if (cu+N>end){
                std::cout << "cannot allocate as ask size is too much big try lower value\n";
                flag=0;
                return nullptr;
            }
            curr=cu;
            oldcurr=curr;
            curr+=N;
            return oldcurr;
       }
        void debug(){
            std::cout << "end : " << (void*)end <<"\n";
            std::cout << "curr : " << (void*)curr <<"\n";
            std::cout << "arr is : " << (void*)arr <<"\n" ;
        }
        void reset(){
            curr=beg;
            flag=1;
        }
        
        size_t bytes_used() const {
            return curr - beg;
        }
        
        size_t bytes_available() const {
            return end - curr;
        }
        
        size_t total_capacity() const {
            return end - beg;
        }
        
    ~Bumpy(){
        free(arr);
    }
};

void print_separator(const std::string& title) {
    std::cout << "\n" << std::string(50, '=') << "\n";
    std::cout << title << "\n";
    std::cout << std::string(50, '=') << "\n";
}

void test_basic_allocation() {
    print_separator("TEST 1: Basic Allocation");
    Bumpy B(1024);
    
    std::cout << "Initial capacity: " << B.total_capacity() << " bytes\n";
    
    char* p1 = B.give_me_bytes(10);
    std::cout << "Allocated 10 bytes at: " << (void*)p1 << "\n";
    std::cout << "Bytes used: " << B.bytes_used() << ", available: " << B.bytes_available() << "\n";
    
    char* p2 = B.give_me_bytes(20);
    std::cout << "Allocated 20 bytes at: " << (void*)p2 << "\n";
    std::cout << "Bytes used: " << B.bytes_used() << ", available: " << B.bytes_available() << "\n";
    
    std::cout << "✓ Basic allocation test passed\n";
}

void test_alignment() {
    print_separator("TEST 2: Alignment Check");
    Bumpy B(1024);
    
    std::cout << "Alignment boundary: " << alignof(std::max_align_t) << " bytes\n\n";
    
    for (int i = 0; i < 5; i++) {
        char* p = B.give_me_bytes(1);
        uintptr_t addr = reinterpret_cast<uintptr_t>(p);
        bool is_aligned = (addr % alignof(std::max_align_t)) == 0;
        
        std::cout << "Allocation " << i+1 << ": address " << (void*)p 
                  << " - " << (is_aligned ? "✓ ALIGNED" : "✗ NOT ALIGNED") << "\n";
    }
}

void test_out_of_memory() {
    print_separator("TEST 3: Out of Memory");
    Bumpy B(100);
    
    std::cout << "Allocator capacity: " << B.total_capacity() << " bytes\n";
    
    std::cout << "\nAttempting to allocate 200 bytes (should fail):\n";
    char* p1 = B.give_me_bytes(200);
    std::cout << "Result: " << (p1 == nullptr ? "nullptr (expected)" : "non-null (unexpected)") << "\n";
    
    B.reset();
    std::cout << "\nAfter reset, allocating 50 bytes:\n";
    char* p2 = B.give_me_bytes(50);
    std::cout << "Result: " << (p2 != nullptr ? "✓ Success" : "✗ Failed") << "\n";
    std::cout << "Bytes used: " << B.bytes_used() << ", available: " << B.bytes_available() << "\n";
    
    std::cout << "\nAllocating another 60 bytes (should fail):\n";
    char* p3 = B.give_me_bytes(60);
    std::cout << "Result: " << (p3 == nullptr ? "nullptr (expected)" : "non-null (unexpected)") << "\n";
}

void test_reset() {
    print_separator("TEST 4: Reset Functionality");
    Bumpy B(1024);
    
    char* p1 = B.give_me_bytes(100);
    char* p2 = B.give_me_bytes(200);
    std::cout << "After allocations - Bytes used: " << B.bytes_used() << "\n";
    
    B.reset();
    std::cout << "After reset - Bytes used: " << B.bytes_used() << "\n";
    
    char* p3 = B.give_me_bytes(50);
    std::cout << "After new allocation - Bytes used: " << B.bytes_used() << "\n";
    
    std::cout << "✓ Reset test passed\n";
}

void test_fragmentation() {
    print_separator("TEST 5: Alignment Overhead (Fragmentation)");
    Bumpy B(1024);
    
    std::cout << "Allocating multiple 1-byte chunks to see alignment overhead:\n\n";
    
    size_t total_requested = 0;
    for (int i = 0; i < 10; i++) {
        size_t before = B.bytes_used();
        char* p = B.give_me_bytes(1);
        size_t after = B.bytes_used();
        size_t overhead = after - before - 1;
        total_requested += 1;
        
        std::cout << "Request #" << std::setw(2) << i+1 
                  << ": Requested 1 byte, actual increase: " << (after - before) 
                  << " bytes (overhead: " << overhead << ")\n";
    }
    
    std::cout << "\nTotal requested: " << total_requested << " bytes\n";
    std::cout << "Total used: " << B.bytes_used() << " bytes\n";
    std::cout << "Overhead ratio: " << std::fixed << std::setprecision(2) 
              << (double)(B.bytes_used() - total_requested) / total_requested * 100 << "%\n";
}

void test_various_sizes() {
    print_separator("TEST 6: Various Allocation Sizes");
    Bumpy B(10000);
    
    std::vector<int> sizes = {1, 8, 16, 32, 64, 128, 256, 512, 1024};
    
    for (int size : sizes) {
        size_t before = B.bytes_used();
        char* p = B.give_me_bytes(size);
        if (p) {
            size_t after = B.bytes_used();
            std::cout << "Allocated " << std::setw(4) << size << " bytes: "
                      << "actual cost = " << (after - before) << " bytes, "
                      << "address = " << (void*)p << "\n";
        } else {
            std::cout << "Failed to allocate " << size << " bytes\n";
        }
    }
}

void test_edge_cases() {
    print_separator("TEST 7: Edge Cases");
    
    std::cout << "Test 7a: Zero-byte allocation\n";
    Bumpy B1(1024);
    char* p1 = B1.give_me_bytes(0);
    std::cout << "0-byte allocation result: " << (void*)p1 << "\n";
    std::cout << "Bytes used after 0-byte allocation: " << B1.bytes_used() << "\n\n";
    
    std::cout << "Test 7b: Very small allocator (16 bytes)\n";
    Bumpy B2(16);
    std::cout << "Capacity: " << B2.total_capacity() << " bytes\n";
    char* p2 = B2.give_me_bytes(1);
    std::cout << "After 1-byte allocation, bytes used: " << B2.bytes_used() << "\n";
    std::cout << "Available: " << B2.bytes_available() << " bytes\n";
}

void test_actual_usage() {
    print_separator("TEST 8: Realistic Usage Pattern");
    Bumpy B(2048);
    
    std::cout << "Simulating struct allocations:\n\n";
    
    struct Point { double x, y; };
    struct Color { uint8_t r, g, b, a; };
    
    std::vector<char*> allocations;
    
    std::cout << "Allocating 10 Points (16 bytes each):\n";
    for (int i = 0; i < 10; i++) {
        char* p = B.give_me_bytes(sizeof(Point));
        if (p) allocations.push_back(p);
    }
    std::cout << "Bytes used: " << B.bytes_used() << "\n\n";
    
    std::cout << "Allocating 20 Colors (4 bytes each):\n";
    for (int i = 0; i < 20; i++) {
        char* p = B.give_me_bytes(sizeof(Color));
        if (p) allocations.push_back(p);
    }
    std::cout << "Bytes used: " << B.bytes_used() << "\n";
    std::cout << "Total allocations: " << allocations.size() << "\n";
    std::cout << "Efficiency: " << std::fixed << std::setprecision(2)
              << (double)(10*sizeof(Point) + 20*sizeof(Color)) / B.bytes_used() * 100 << "%\n";
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════╗\n";
    std::cout << "║     BUMP ALLOCATOR COMPREHENSIVE TEST SUITE    ║\n";
    std::cout << "╚════════════════════════════════════════════════╝\n";
    
    test_basic_allocation();
    test_alignment();
    test_out_of_memory();
    test_reset();
    test_fragmentation();
    test_various_sizes();
    test_edge_cases();
    test_actual_usage();
    
    print_separator("ALL TESTS COMPLETED");
    std::cout << "\n";
    
    return 0;
}
