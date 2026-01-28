#include <iostream>
#include <vector>
#include <cstdint>
#include <cstddef>
class Bumpy{
    private:
        char* end{nullptr};
        char* curr{nullptr};
        char* old_curr{nullptr};
        char* arr;
    public:
        Bumpy(int N){
            arr=(char*)malloc(N*sizeof(char));
            end=arr+N;
            curr=arr;
        }
       char * give_me_bytes(int N){
            if (curr+N>=end){
                std::cout << "not possible\n";
                return nullptr;
            }
            old_curr=curr;
            curr=curr+N;
            int alli=alignof(std::max_align_t);
            int *al=&alli;
            if (*curr/ *al!=0){
                curr+=(16-(* curr% *al));
            }
            return old_curr;
        }
        void debug(){
            std:: cout << "end : " << end <<"\n";
            std:: cout << "curr : " << curr <<"\n";
           std:: cout << "old_curr: " << old_curr << "\n";
            std:: cout << "arr is : " << arr <<"\n" ;
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
    int n;
    std::cout << "enter the size of array : \n";
    std::cin>>n;
    int* a=B.give_me_bytes(n);
    B.debug();
    return 0;
}
