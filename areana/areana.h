#pragma once
class Chunk{
    private:
     char* end{nullptr};
    char* start{nullptr};
    char* curr{nullptr};
    char* arr;
    public:
    Chunk(int N);
    char* alloc(int N,int allign);
    Chunk(const Chunk&)=delete;
    Chunk&operator(const Chunk&)=delete;
    Chunk(Chunk&&x)noexcept;
    Chunk&operator=(Chunk&&x)noexcept;
    void reset();
    ~Chunk();
};
class Areana{
    private:
        std::vector<&Chunk>areana;
        int extra{};
        int ali{4};
    public:
        Areana(int n,int a);
        char *allocate(int byt);
        ~Areana();
};
