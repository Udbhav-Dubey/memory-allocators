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
        slab(size_t ,size_t );
        char*allocate();
        void pfree(char*);
        bool full();
        bool partial();
        bool empty();
        void print_debug();
        bool checkrange(char*);
        ~slab();
};
class cache{
    std::vector<slab*>used;
    std::vector<slab*>empty;
    std::vector<slab*>partial;
   size_t tsize{};
   size_t total_size{};
   void debug_type(std::vector<slab*>);
    public:
   cache(const cache&)=delete;
   cache operator=(const cache&)=delete;
   void debug();
   cache(size_t ,size_t );
   char*allocate();
   void cfree(char*);
   ~cache();
};
class slabAllocator{
    private:
        size_t totalsize{};
        std::unordered_map<size_t,cache*>stc; // size to cache
        std::unordered_map<char*,cache*>ptc; // ptr to cache
        void findsize(size_t& );
    public:
        slabAllocator(size_t );
        char*allocate(size_t );
        void free(char*);
        ~slabAllocator();
        void debug();
};
