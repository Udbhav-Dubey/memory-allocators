#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <algorithm>
#include <random>
#include "pool.h"

// ── the object being allocated ─────────────────────────────────────────────
struct Node {
    int   id;
    float x, y, z;
    Node* next{nullptr};
};

static constexpr int k_num_allocs = 100'000;
static constexpr std::size_t k_node_size = sizeof(Node);

// ── tiny RAII timer ────────────────────────────────────────────────────────
struct Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point t0 = Clock::now();
    long long elapsed_us() const {
        return std::chrono::duration_cast<std::chrono::microseconds>(
                   Clock::now() - t0).count();
    }
};

// ── result printer ─────────────────────────────────────────────────────────
static void print_row(const char* label, long long pool_us, long long new_us) {
    double speedup = static_cast<double>(new_us) /
                     static_cast<double>(pool_us ? pool_us : 1);
    std::cout << "  " << label << "\n"
              << "    pool       : " << pool_us << " us\n"
              << "    new/delete : " << new_us  << " us\n"
              << "    speedup    : " << speedup << "x\n\n";
}

// ══════════════════════════════════════════════════════════════════════════
// TEST 1 — alloc only
//          pool equivalent of the arena benchmark — no frees, just allocate
//          until the pool is full, then drain it with pfree to reset state
// ══════════════════════════════════════════════════════════════════════════
static void bench_alloc_only(Pool& pool) {
    std::vector<char*> ptrs;
    ptrs.reserve(k_num_allocs);

    Timer t_pool;
    for (int i = 0; i < k_num_allocs; ++i)
        ptrs.push_back(pool.allocate(k_node_size));
    long long pool_us = t_pool.elapsed_us();

    // drain so pool is reusable for next test
    for (char* p : ptrs) pool.pfree(p);

    // ── new baseline ──
    std::vector<Node*> heap_ptrs;
    heap_ptrs.reserve(k_num_allocs);

    Timer t_new;
    for (int i = 0; i < k_num_allocs; ++i)
        heap_ptrs.push_back(new Node());
    long long new_us = t_new.elapsed_us();

    for (Node* p : heap_ptrs) delete p;

    print_row("alloc-only  (N allocs, no free measured)", pool_us, new_us);
}

// ══════════════════════════════════════════════════════════════════════════
// TEST 2 — alloc then free, sequential order
//          canonical pool workload — measures the full round-trip cost
// ══════════════════════════════════════════════════════════════════════════
static void bench_alloc_free_sequential(Pool& pool) {
    std::vector<char*> ptrs;
    ptrs.reserve(k_num_allocs);

    Timer t_pool;
    for (int i = 0; i < k_num_allocs; ++i)
        ptrs.push_back(pool.allocate(k_node_size));
    for (char* p : ptrs)
        pool.pfree(p);
    long long pool_us = t_pool.elapsed_us();

    // ── new/delete baseline ──
    std::vector<Node*> heap_ptrs;
    heap_ptrs.reserve(k_num_allocs);

    Timer t_new;
    for (int i = 0; i < k_num_allocs; ++i)
        heap_ptrs.push_back(new Node());
    for (Node* p : heap_ptrs)
        delete p;
    long long new_us = t_new.elapsed_us();

    print_row("alloc + free  (sequential order)", pool_us, new_us);
}

// ══════════════════════════════════════════════════════════════════════════
// TEST 3 — alloc then free, random order
//          stresses the free-list relink path;
//          also punishes new/delete via TLB/cache thrash on the heap
// ══════════════════════════════════════════════════════════════════════════
static void bench_alloc_free_random(Pool& pool) {
    std::vector<char*> ptrs;
    ptrs.reserve(k_num_allocs);
    for (int i = 0; i < k_num_allocs; ++i)
        ptrs.push_back(pool.allocate(k_node_size));

    std::vector<int> order(k_num_allocs);
    std::iota(order.begin(), order.end(), 0);
    std::mt19937 rng(42);
    std::shuffle(order.begin(), order.end(), rng);

    Timer t_pool;
    for (int idx : order)
        pool.pfree(ptrs[idx]);
    long long pool_us = t_pool.elapsed_us();

    // ── new/delete baseline ──
    std::vector<Node*> heap_ptrs;
    heap_ptrs.reserve(k_num_allocs);
    for (int i = 0; i < k_num_allocs; ++i)
        heap_ptrs.push_back(new Node());

    std::shuffle(order.begin(), order.end(), rng);

    Timer t_new;
    for (int idx : order)
        delete heap_ptrs[idx];
    long long new_us = t_new.elapsed_us();

    print_row("free-only  (random order, after full alloc)", pool_us, new_us);
}

// ══════════════════════════════════════════════════════════════════════════
// TEST 4 — churn: interleaved burst alloc/free
//          most realistic workload — free-list stays warm, slots get reused
// ══════════════════════════════════════════════════════════════════════════
static void bench_churn(Pool& pool) {
    static constexpr int k_burst  = 256;
    static constexpr int k_rounds = k_num_allocs / k_burst;

    std::vector<char*> live(k_burst, nullptr);

    Timer t_pool;
    for (int r = 0; r < k_rounds; ++r) {
        for (int i = 0; i < k_burst; ++i)
            live[i] = pool.allocate(k_node_size);
        for (int i = 0; i < k_burst; ++i)
            pool.pfree(live[i]);
    }
    long long pool_us = t_pool.elapsed_us();

    // ── new/delete baseline ──
    std::vector<Node*> heap_live(k_burst, nullptr);

    Timer t_new;
    for (int r = 0; r < k_rounds; ++r) {
        for (int i = 0; i < k_burst; ++i)
            heap_live[i] = new Node();
        for (int i = 0; i < k_burst; ++i)
            delete heap_live[i];
    }
    long long new_us = t_new.elapsed_us();

    print_row("churn  (burst alloc + burst free, interleaved)", pool_us, new_us);
}

// ══════════════════════════════════════════════════════════════════════════
int main() {
    // Pool(total_space, slots) — matches your constructor signature
    // total_space *2 gives headroom after internal alignment rounding
    Pool pool(k_num_allocs * k_node_size * 2, k_num_allocs);

    std::cout << "Pool allocator benchmark  |  N = " << k_num_allocs
              << "  |  node = " << k_node_size << " bytes\n"
              << std::string(60, '-') << "\n\n";

    bench_alloc_only(pool);
    bench_alloc_free_sequential(pool);
    bench_alloc_free_random(pool);
    bench_churn(pool);

    std::cout << std::string(60, '-') << "\n"
              << "compile: g++ -O2 -std=c++17 pool_tester.cpp pool.cpp -o pool_bench\n";
    return 0;
}
