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
//                std::cout << "checker of while inside check\n";
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
int main (){
    int bytes;
    std::cout << "How many bytes : \n";
    std::cin >> bytes;
    std::cout << "enter the allignment in power of 2 please : \n";
    int ask_a;
    std::cin>>ask_a;
    Areana are(bytes,ask_a);
    //while(true){
    std::cout << "enter the number of bytes you want : \n";
    int ask_b;
    std::cin>>ask_b;
    char* a=are.allocate(ask_b);
    if (a==nullptr){
        std::cout << "allocation failed\n";
    }
    else std::cout << "allocation occured\n";

    //}
    return 0;
}
