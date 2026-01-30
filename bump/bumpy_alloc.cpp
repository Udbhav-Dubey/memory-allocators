#include <iostream>
#include <vector>
#include <cstdint>
#include <cstddef>
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
            // first check is it even possible 
            // first check aayega allignement ka 
            // agar allign hai to theek nahi to allign karo 
            // allign fir oom checking ki ab possible or not 
            // if possible plus n karo aur return old one okay
            // allignement is a destination there we need to reach before allocating size 
            // okay 
            if (curr+N>end){
                std::cout << "cannot allocate this much memory as it would lead to out of memory\n";
                flag=0;
                return nullptr;
            }
            char *oldcurr{nullptr};
            uintptr_t mask=alignof(std::max_align_t)-1;
            // i am gonna allign never the less even if its already alligned 
            uintptr_t addr=reinterpret_cast<uintptr_t>(curr);
            addr+=mask;
            addr&=~(mask);
            curr=reinterpret_cast<char*>(addr);
            if (curr>end){std::cout << "cannot allocate this much memory allign issue\n";flag=0;return nullptr;}
            oldcurr=curr;
            curr+=N;
            return oldcurr;
       }
        void debug(){
            std:: cout << "end : " << (void*)end <<"\n";
            std:: cout << "curr : " << (void*)curr <<"\n";
            std:: cout << "arr is : " << (void*)arr <<"\n" ;
        }
        void reset(){
            curr=beg;
        }
    ~Bumpy(){
        free(arr);
    }
};
int main (){
    int user_bytes;
    std::cout << "enter the number of bytes you want : \n";
    std::cin>>user_bytes;
    Bumpy B(user_bytes);
    while(B.flag){
    int n;
    std::cout << "you ask for bytes I shall give : " << "\n";
    std::cin>>n;
    char* a=B.give_me_bytes(n);
    std::cout << "what did i get " << (int*)a <<"\n";
    B.debug();
    }
    return 0;
}
