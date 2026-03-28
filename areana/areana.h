#pragma once
#include <vector>
#include <cstdint>
class Chunk{
    private:
     char* end{nullptr};
    char* start{nullptr};
    char* curr{nullptr};
    char* arr;
    public:
    Chunk(size_t N);
    char* alloc(size_t N,size_t allign);
    Chunk(const Chunk&)=delete;
    Chunk&operator=(const Chunk&)=delete;
    Chunk(Chunk&&x)noexcept;
    Chunk&operator=(Chunk&&x)noexcept;
    void reset();
    ~Chunk();
};
class Areana{
    private:
        std::vector<Chunk>areana;
        size_t extra{};
        size_t ali{4};
    public:
        Areana(size_t n,size_t a);
        char *allocate(size_t byt);
        ~Areana();
};
