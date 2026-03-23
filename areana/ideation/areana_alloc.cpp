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
/*        if (curr+N>end){
            return nullptr;
        }*/
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
    Chunk(const Chunk&) = delete; // delete constructor still not working 
    Chunk& operator=(const Chunk&)=delete;
    Chunk& operator=(Chunk&&);
    Chunk(Chunk && x){
        arr=x.arr;
        x.arr=nullptr;
        curr=x.curr;
        x.curr=nullptr;
        start=x.start;
        x.start=nullptr;
        end=x.end;
        x.end=nullptr;
    }
    void reset(){
        curr=start;
    }
    ~Chunk(){
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
       // Chunk ch(n);
        areana.emplace_back(n);
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
           //     Chunk ch(byt+extra);
                idx_last++;
                areana.emplace_back(byt+extra);
                temp=areana[idx_last].alloc(byt,ali);
//                std::cout << "checker of while inside check\n";
            }
        }
            return temp;
    }
    ~Areana(){
        std::cout << "size so far : " << areana.size() << "\n";
        for (auto &c:areana){
            c.reset();
        }
        areana.clear();
    }
};
void test1(){
int bytes;
    std::cout << "How many bytes : \n";
    std::cin >> bytes;
    std::cout << "enter the allignment in power of 2 please : \n";
    int ask_a;
    std::cin>>ask_a;
    Areana are(bytes,ask_a);
    while(true){
    std::cout << "enter the number of bytes you want : \n";
    int ask_b;
    std::cin>>ask_b;
    char* a=are.allocate(ask_b);
    if (a==nullptr){
        std::cout << "allocation failed\n";
    }
    else std::cout << "allocation occured\n";
    }
}
void test2(){
Areana a(64, 8);
char* p1 = a.allocate(16);
char* p2 = a.allocate(16);
}
void test2_1(){
    Areana a(64, 8);
char* p = a.allocate(1);
}
int main (){
    test2_1();
    return 0;
}
