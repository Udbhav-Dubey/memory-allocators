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
            if (curr+N>=end){
                std::cout << "not possible\n";
                flag=0;
                return nullptr;
            }
            int alli=alignof(std::max_align_t);
            char* old_curr=curr;
            if (N%alli==0){
                curr+=N;
            } 
            else {
                int x=N%alli;
                 N+=(alli-x);
                curr+=N;
            }
            return old_curr;
        }
        void debug(){
            std:: cout << "end : " << (int*)end <<"\n";
            std:: cout << "curr : " << (int*)curr <<"\n";
            std:: cout << "arr is : " << (int*)arr <<"\n" ;
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
