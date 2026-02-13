#include <iostream>
#include <vector>
#include <cstdint>

class Chunk{
    private:
    char* end{nullptr};
    char* start{nullptr};
    char* curr{nullptr};
    char* arr;
    bool flag{0};
    public:
    Chunk(int N){
        arr=(char*)malloc(sizeof(char)*N);
        start=arr;
        end=arr+N;
        curr=arr;
    }
    char* alloc(int N,int allign){
        if (curr+N>end){
            return nullptr;
        }
        if (allign<0||(allign&(allign-1))!=0){
            flag=1;
            return nullptr;
        }
        uintptr_t mask=allign-1;
        uintptr_t ucur=reinterpret_cast<uintptr_t>(curr);
        ucur=(ucur+mask) & ~(mask);
        char* check=reinterpret_cast<char*>(ucur);
        if (check>end || check+N>end){
            return nullptr;
        }
        curr=check;
        char*oldcurr(curr);
        curr+=N;
        return oldcurr;
    }
    bool getflag(){
        return flag;
    }
    void reset(){
        curr=start;
        free(arr);
    }
};

class Areana{
    private:
        std::vector<Chunk>areana;
        int idx_last{-1};
        int extra{};
        int ali{4};
    public:
    Areana(int n,int a){
        Chunk ch(n);
        areana.push_back(ch);  
        idx_last++;
        ali=a;
        extra=n;
    }
    char* allocate(int byt){
       char*temp=areana[idx_last].alloc(byt,ali);
        if (temp==nullptr){
            if (areana[idx_last].getflag()==1){
                std::cout << "align issue\n";
                return nullptr;
            }
            while(temp==nullptr){
                Chunk ch(byt+extra);
                idx_last++;
                areana.push_back(ch);
                temp=areana[idx_last].alloc(byt,ali);
            }
        }
            return temp;
    }
    ~Areana(){
        std::cout << "size so far : " << areana.size() << "\n";
        for (auto c:areana){
            c.reset();
        }
        areana.clear();
    }
};

// TESTER FUNCTION - Tests various scenarios to find bugs
void tester() {
    std::cout << "\n========== STARTING TESTS ==========\n\n";
    
    // Test 1: Basic allocation
    std::cout << "Test 1: Basic allocation (100 bytes, alignment 4)\n";
    {
        Areana arena1(100, 4);
        char* ptr = arena1.allocate(50);
        if (ptr) {
            std::cout << "✓ Allocation successful\n";
        } else {
            std::cout << "✗ FAILED: Allocation returned nullptr\n";
        }
    }
    
    // Test 2: Multiple allocations
    std::cout << "\nTest 2: Multiple allocations\n";
    {
        Areana arena2(100, 4);
        char* ptr1 = arena2.allocate(20);
        char* ptr2 = arena2.allocate(30);
        char* ptr3 = arena2.allocate(40);
        std::cout << "Ptr1: " << (void*)ptr1 << "\n";
        std::cout << "Ptr2: " << (void*)ptr2 << "\n";
        std::cout << "Ptr3: " << (void*)ptr3 << "\n";
    }
    
    // Test 3: Allocation exceeding initial chunk size
    std::cout << "\nTest 3: Allocation exceeding chunk size (should create new chunk)\n";
    {
        Areana arena3(50, 4);
        char* ptr = arena3.allocate(100);
        if (ptr) {
            std::cout << "✓ Large allocation successful\n";
        } else {
            std::cout << "✗ FAILED: Large allocation returned nullptr\n";
        }
    }
    
    // Test 4: Different alignments
    std::cout << "\nTest 4: Testing different alignments (1, 2, 4, 8, 16)\n";
    int alignments[] = {1, 2, 4, 8, 16};
    for (int align : alignments) {
        Areana arena(100, align);
        char* ptr = arena.allocate(10);
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        if (ptr && (addr % align == 0)) {
            std::cout << "✓ Alignment " << align << " correct\n";
        } else if (ptr) {
            std::cout << "✗ Alignment " << align << " INCORRECT - address: " << (void*)ptr << "\n";
        } else {
            std::cout << "✗ Alignment " << align << " failed - nullptr\n";
        }
    }
    
    // Test 5: Invalid alignment (not power of 2)
    std::cout << "\nTest 5: Invalid alignment (3 - not power of 2)\n";
    {
        Areana arena5(100, 3);
        char* ptr = arena5.allocate(10);
        if (ptr == nullptr) {
            std::cout << "✓ Correctly rejected invalid alignment\n";
        } else {
            std::cout << "✗ FAILED: Should reject alignment=3\n";
        }
    }
    
    // Test 6: Zero-byte allocation
    std::cout << "\nTest 6: Zero-byte allocation\n";
    {
        Areana arena6(100, 4);
        char* ptr = arena6.allocate(0);
        std::cout << "Result: " << (void*)ptr << "\n";
    }
    
    // Test 7: Very small chunk with large allocation request
    std::cout << "\nTest 7: Very small initial chunk (10 bytes) requesting 100 bytes\n";
    {
        Areana arena7(10, 4);
        char* ptr = arena7.allocate(100);
        if (ptr) {
            std::cout << "✓ Successfully created new chunk\n";
        } else {
            std::cout << "✗ FAILED: Should create new chunk\n";
        }
    }
    
    // Test 8: Reset and reuse
    std::cout << "\nTest 8: Reset and reuse arena\n";
    {
        Areana arena8(100, 4);
        char* ptr1 = arena8.allocate(50);
        std::cout << "First allocation: " << (void*)ptr1 << "\n";
        
        // Note: No reset method available, so this test shows limitation
        char* ptr2 = arena8.allocate(50);
        std::cout << "Second allocation: " << (void*)ptr2 << "\n";
        if (ptr1 && ptr2 && ptr1 != ptr2) {
            std::cout << "✓ Multiple allocations work correctly\n";
        }
    }
    
    // Test 9: Negative alignment
    std::cout << "\nTest 9: Negative alignment\n";
    {
        Areana arena9(100, -4);
        char* ptr = arena9.allocate(10);
        if (ptr == nullptr) {
            std::cout << "✓ Correctly rejected negative alignment\n";
        } else {
            std::cout << "✗ FAILED: Should reject negative alignment\n";
        }
    }
    
    // Test 10: Write to allocated memory
    std::cout << "\nTest 10: Write to allocated memory\n";
    {
        Areana arena10(100, 4);
        char* ptr = arena10.allocate(10);
        if (ptr) {
            for (int i = 0; i < 10; i++) {
                ptr[i] = 'A' + i;
            }
            std::cout << "✓ Successfully wrote to memory: ";
            for (int i = 0; i < 10; i++) {
                std::cout << ptr[i];
            }
            std::cout << "\n";
        }
    }
    
    // Test 11: Stress test - many small allocations
    std::cout << "\nTest 11: Stress test - many small allocations\n";
    {
        Areana arena(1024, 8);
        std::vector<char*> ptrs;
        for (int i = 0; i < 100; i++) {
            char* p = arena.allocate(10);
            if (p) ptrs.push_back(p);
        }
        std::cout << "✓ Allocated " << ptrs.size() << " blocks\n";
    }

    // Test 12: Maximum alignment (64 bytes)
    std::cout << "\nTest 12: Maximum alignment (64 bytes)\n";
    {
        Areana arena(1024, 64);
        char* p = arena.allocate(100);
        if (p) {
            uintptr_t addr = reinterpret_cast<uintptr_t>(p);
            if (addr % 64 == 0) {
                std::cout << "✓ 64-byte alignment correct\n";
            } else {
                std::cout << "✗ 64-byte alignment INCORRECT\n";
            }
        } else {
            std::cout << "✗ Allocation failed\n";
        }
    }

    // Test 13: Allocation pattern (varied sizes)
    std::cout << "\nTest 13: Allocation pattern (varied sizes)\n";
    {
        Areana arena(2048, 8);
        char* p1 = arena.allocate(1);
        char* p2 = arena.allocate(100);
        char* p3 = arena.allocate(1000);
        char* p4 = arena.allocate(50);
        if (p1 && p2 && p3 && p4) {
            std::cout << "✓ Varied size allocations successful\n";
        } else {
            std::cout << "✗ Some allocations failed\n";
        }
    }
    
    std::cout << "\n========== TESTS COMPLETE ==========\n\n";
}

int main() {
    // Run automated tests
    tester();
/*    
    // Original interactive code
    std::cout << "\n========== INTERACTIVE MODE ==========\n";
    int bytes;
    std::cout << "How many bytes : \n";
    std::cin >> bytes;
    std::cout << "enter the allignment in power of 2 please : \n";
    int ask_a;
    std::cin >> ask_a;
    Areana are(bytes, ask_a);
    std::cout << "enter the number of bytes you want : \n";
    int ask_b;
    std::cin >> ask_b;
    char* a = are.allocate(ask_b);
    if (a == nullptr) {
        std::cout << "allocation failed\n";
    } else {
        std::cout << "allocation occured\n";
    }
  */  
    return 0;
}
