#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include <chrono>
#include "slab.h"

static int g_passed = 0;
static int g_failed = 0;

struct Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point t0 = Clock::now();
    long long elapsed_us() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   Clock::now() - t0).count();
    }
};

static void check(bool cond, const char* label) {
    if (cond) { std::cout << "  [PASS] " << label << "\n"; ++g_passed; }
    else       { std::cout << "  [FAIL] " << label << "\n"; ++g_failed; }
}

static void section(const char* title) {
    std::cout << "\n" << title << "\n" << std::string(50, '-') << "\n";
}

static void bench_row(const char* label, long long slab_us, long long new_us) {
    double speedup = static_cast<double>(new_us) /
                     static_cast<double>(slab_us ? slab_us : 1);
    std::cout << "  " << label << "\n"
              << "    slabAllocator : " << slab_us << " us\n"
              << "    new/delete    : " << new_us  << " us\n"
              << "    speedup       : " << speedup << "x\n\n";
}

static void test_single_alloc_free() {
    section("1. single alloc / free");
    slabAllocator sa(1024 * 1024);

    char* p = sa.allocate(32);
    check(p != nullptr, "allocate(32) returns non-null");

    sa.free(p);
    check(true, "free(p) does not crash");
}

static void test_different_sizes() {
    section("2. different sizes routed correctly");
    slabAllocator sa(4 * 1024 * 1024);

    char* p8  = sa.allocate(8);
    char* p16 = sa.allocate(16);
    char* p32 = sa.allocate(32);
    char* p64 = sa.allocate(64);

    check(p8  != nullptr, "allocate(8)  non-null");
    check(p16 != nullptr, "allocate(16) non-null");
    check(p32 != nullptr, "allocate(32) non-null");
    check(p64 != nullptr, "allocate(64) non-null");

    sa.free(p8);
    sa.free(p16);
    sa.free(p32);
    sa.free(p64);
    check(true, "all frees complete without crash");
}

static void test_write_through() {
    section("3. write-through (memory is usable)");
    slabAllocator sa(1024 * 1024);

    static constexpr int N  = 128;
    static constexpr int BS = 32;
    std::vector<char*> ptrs;
    ptrs.reserve(N);

    for (int i = 0; i < N; ++i) ptrs.push_back(sa.allocate(BS));

    bool ok = true;
    for (int i = 0; i < N; ++i)
        std::fill(ptrs[i], ptrs[i] + BS, static_cast<char>(i & 0xFF));

    for (int i = 0; i < N; ++i)
        for (int b = 0; b < BS; ++b)
            if (ptrs[i][b] != static_cast<char>(i & 0xFF)) { ok = false; break; }

    check(ok, "all written bytes read back correctly");

    for (char* p : ptrs) sa.free(p);
}

static void test_sequential_alloc_free() {
    section("4. sequential alloc then free");
    slabAllocator sa(4 * 1024 * 1024);

    static constexpr int N = 1000;
    std::vector<char*> ptrs;
    ptrs.reserve(N);

    for (int i = 0; i < N; ++i) ptrs.push_back(sa.allocate(32));

    bool all_valid = true;
    for (char* p : ptrs) if (!p) { all_valid = false; break; }
    check(all_valid, "all 1000 allocations non-null");

    for (char* p : ptrs) sa.free(p);
    check(true, "all sequential frees complete");
}

static void test_random_free_order() {
    section("5. random free order");
    slabAllocator sa(4 * 1024 * 1024);

    static constexpr int N = 1000;
    std::vector<char*> ptrs;
    ptrs.reserve(N);
    for (int i = 0; i < N; ++i) ptrs.push_back(sa.allocate(32));

    std::vector<int> order(N);
    std::iota(order.begin(), order.end(), 0);
    std::mt19937 rng(42);
    std::shuffle(order.begin(), order.end(), rng);

    for (int idx : order) sa.free(ptrs[idx]);
    check(true, "random-order free completes without crash");

    char* p = sa.allocate(32);
    check(p != nullptr, "allocate works again after random free");
    sa.free(p);
}

static void test_reuse_after_free() {
    section("6. slot reuse after free");
    slabAllocator sa(1024 * 1024);

    static constexpr int N = 256;
    std::vector<char*> r1, r2;
    r1.reserve(N); r2.reserve(N);

    for (int i = 0; i < N; ++i) r1.push_back(sa.allocate(32));
    for (char* p : r1) sa.free(p);
    for (int i = 0; i < N; ++i) r2.push_back(sa.allocate(32));

    bool all_valid = true;
    for (char* p : r2) if (!p) { all_valid = false; break; }
    check(all_valid, "all second-round ptrs non-null");

    for (char* p : r2) sa.free(p);
}

static void test_mixed_sizes_interleaved() {
    section("7. mixed sizes interleaved alloc/free");
    slabAllocator sa(4 * 1024 * 1024);

    std::vector<std::pair<char*, size_t>> live;
    size_t sizes[] = {8, 16, 32, 64, 128};
    std::mt19937 rng(7);

    for (int i = 0; i < 500; ++i) {
        size_t sz = sizes[rng() % 5];
        char* p = sa.allocate(sz);
        if (p) live.push_back({p, sz});

        if (live.size() > 50) {
            sa.free(live.front().first);
            live.erase(live.begin());
        }
    }
    for (auto [p, _] : live) sa.free(p);
    check(true, "mixed-size interleaved alloc/free survives");
}

static void bench_alloc_only() {
    section("bench — alloc only");
    static constexpr int    N  = 100'000;
    static constexpr size_t BS = 32;

    slabAllocator sa(static_cast<size_t>(N) * BS * 4);
    std::vector<char*> ptrs;
    ptrs.reserve(N);

    Timer t_sa;
    for (int i = 0; i < N; ++i) ptrs.push_back(sa.allocate(BS));
    long long sa_us = t_sa.elapsed_us();

    for (char* p : ptrs) sa.free(p);

    std::vector<char*> heap;
    heap.reserve(N);
    Timer t_new;
    for (int i = 0; i < N; ++i) heap.push_back(new char[BS]);
    long long new_us = t_new.elapsed_us();
    for (char* p : heap) delete[] p;

    bench_row("alloc only  (N allocs, no free measured)", sa_us, new_us);
}

static void bench_alloc_free_sequential() {
    section("bench — alloc + free sequential");
    static constexpr int    N  = 100'000;
    static constexpr size_t BS = 32;

    slabAllocator sa(static_cast<size_t>(N) * BS * 4);
    std::vector<char*> ptrs;
    ptrs.reserve(N);

    Timer t_sa;
    for (int i = 0; i < N; ++i) ptrs.push_back(sa.allocate(BS));
    for (char* p : ptrs) sa.free(p);
    long long sa_us = t_sa.elapsed_us();

    std::vector<char*> heap;
    heap.reserve(N);
    Timer t_new;
    for (int i = 0; i < N; ++i) heap.push_back(new char[BS]);
    for (char* p : heap) delete[] p;
    long long new_us = t_new.elapsed_us();

    bench_row("alloc + free  (sequential)", sa_us, new_us);
}

static void bench_churn() {
    section("bench — churn (interleaved burst)");
    static constexpr int    k_burst  = 256;
    static constexpr int    k_rounds = 400;
    static constexpr size_t BS       = 32;

    slabAllocator sa(static_cast<size_t>(k_burst) * BS * 4);
    std::vector<char*> live(k_burst);

    Timer t_sa;
    for (int r = 0; r < k_rounds; ++r) {
        for (int i = 0; i < k_burst; ++i) live[i] = sa.allocate(BS);
        for (int i = 0; i < k_burst; ++i) sa.free(live[i]);
    }
    long long sa_us = t_sa.elapsed_us();

    std::vector<char*> heap(k_burst);
    Timer t_new;
    for (int r = 0; r < k_rounds; ++r) {
        for (int i = 0; i < k_burst; ++i) heap[i] = new char[BS];
        for (int i = 0; i < k_burst; ++i) delete[] heap[i];
    }
    long long new_us = t_new.elapsed_us();

    bench_row("churn  (burst alloc + burst free, interleaved)", sa_us, new_us);
}

static void bench_mixed_sizes() {
    section("bench — mixed sizes");
    static constexpr int N   = 100'000;
    size_t sizes[]           = {8, 16, 32, 64, 128};
    std::mt19937 rng(13);

    slabAllocator sa(static_cast<size_t>(N) * 128 * 4);
    std::vector<char*> ptrs;
    ptrs.reserve(N);

    Timer t_sa;
    for (int i = 0; i < N; ++i) ptrs.push_back(sa.allocate(sizes[rng() % 5]));
    for (char* p : ptrs) sa.free(p);
    long long sa_us = t_sa.elapsed_us();

    std::vector<char*> heap;
    heap.reserve(N);
    std::mt19937 rng2(13);
    Timer t_new;
    for (int i = 0; i < N; ++i) heap.push_back(new char[sizes[rng2() % 5]]);
    for (char* p : heap) delete[] p;
    long long new_us = t_new.elapsed_us();

    bench_row("mixed sizes  (8/16/32/64/128 random, alloc+free)", sa_us, new_us);
}

int main() {
    std::cout << "slabAllocator tests\n" << std::string(50, '=') << "\n";

    test_single_alloc_free();
    test_different_sizes();
    test_write_through();
    test_sequential_alloc_free();
    test_random_free_order();
    test_reuse_after_free();
    test_mixed_sizes_interleaved();

    std::cout << "\n" << std::string(50, '=') << "\n"
              << "results: " << g_passed << " passed, " << g_failed << " failed\n"
              << std::string(50, '=') << "\n";

    bench_alloc_only();
    bench_alloc_free_sequential();
    bench_churn();
    bench_mixed_sizes();

    std::cout << std::string(50, '-') << "\n"
              << "compile: g++ -O2 -std=c++17 slab_tester.cpp slab.cpp -o slab_test\n";
    return g_failed ? 1 : 0;
}
