#pragma once
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <vector>
#include <unordered_map>
#include <utility>
class slab{
    size_t block_size{};
    size_t total_size{};
    int slots{};
    char*base{nullptr};
    char*head{nullptr};
    char*max_size;
    int used_count{};
    public:
    slab(const slab&)=delete;
    slab operator=(const slab&)=delete;
        slab(size_t t,size_t ts);
        char*allocate();
        void pfree(char*ptr_x);
        bool full();
        bool partial();
        bool empty();
        void print_debug();
        bool checkrange(char*x);
        ~slab();
};
class cache{
    std::vector<slab*>used;
    std::vector<slab*>empty;
    std::vector<slab*>partial;
   size_t tsize{};
   size_t total_size{};
   void debug_type(std::vector<slab*>x);
    public:
   cache(const cache&)=delete;
   cache operator=(const cache&)=delete;
   void debug();
   cache(size_t ts,size_t t);
   char*allocate();
   void cfree(char*x);
   ~cache();
};
class slabAllocator{
    private:
        int totalsize{};
        std::unordered_map<int,cache*>mp;
        std::unordered_map<char*,cache*>ptc;
        int findsize(int n);
    public:
        slabAllocator(int ts);
        char*allocate(int x);
        void free(char*x);
        ~slabAllocator();
        void debug();
};
