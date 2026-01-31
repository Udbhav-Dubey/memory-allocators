#include <iostream>
#include <cstdint>
class BumpAllocator{
private:
    char*m_buffer;
    size_t m_buffer_size;
    size_t m_offset;
public :
    BumpAllocator(size_t buffer_size):
        m_buffer_size(buffer_size),m_offset(0){
            m_buffer=new (std::nothrow) char[m_buffer_size];
            if (!m_buffer){
                std::cerr<< "Failed to allocate memory for the buffer." << std::endl;
            }
        }
    ~BumpAllocator(){
        delete[] m_buffer;
    }
    void *allocate(size_t size){
        if (!m_buffer){
            return nullptr;
        }
        size_t aligned_offset=(m_offset+7)&~7;
        if (aligned_offset+size>m_buffer_size){
            std::cerr<<"Error : out of memory in bump allocator." << std::endl;
            return nullptr;
        }
        void *result=m_buffer+aligned_offset;
        m_offset=aligned_offset+size;
        return result;
    }
    void reset(){
        m_offset=0;
    }
};
struct MyData{
    int id;
    double value;
};
int main (){
    BumpAllocator allocator(1024*10);
    int *num1=static_cast<int*>(allocator.allocate(sizeof(int)));
    if (num1){
        *num1=100;
        std::cout<<"Allocated num1 with value : "  << *num1<< std::endl;
    }
    MyData* data1=static_cast<MyData*>(allocator.allocate(sizeof(MyData)));
    if (data1){
        data1->id=42;
        data1->value=3.14;
        std::cout << "Allocated data1 with id : " << data1->id << "and value " << data1->value << std::endl;
    }
    std::cout << "\nResseting the allocator... " << std::endl;
    allocator.reset();
    int*num2=static_cast<int*>(allocator.allocate(sizeof(int)));
    if (num2){
        *num2=200;
        std::cout<<"Re-allocated num2 with new value " << *num2 << std::endl;
    }
    return 0;
}
