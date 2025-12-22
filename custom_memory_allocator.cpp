#include <iostream>
#include <vector>
#include <cstddef>
template<typename T ,size_t Size>
class ArenaAllocator{
private:
    alignas(T) std:: byte m_buffer[Size];
    std::byte* m_ptr;
public :
    using value_type=T;
    AreanAllocator() noexcept:m_ptr(m_buffer){}
    template<typename U,size_t OtherSize>
    AreanaAllocator(const AreanaAllocator<U,OtherSize>&other)noexcept:m_ptr(m_buffer){}
    T* allocate(size_t n){
        const size_t bytes_to_alloc=n*sizeof(T);
        if (m_ptr+bytes_to_alloc>m_buffer+Size){
            std::cerr<<"Areana allocator out of memory !" << std::endl;
        throw std::bad_alloc();
        }
        T* result=reinterpret_cast<T*>(m_ptr);
        m_ptr+=bytes_to_alloc;
        std::cout<<"Areana allocated " << bytes_to_alloc<< "bytes.Pointer at offset " << (m_ptr-m_buffer) << "\n";
        return result;
    }
    void deallocate(T*p,size_t n) noexcept{
        std::cout << "Arena:Deallocate called (no-op)\n.";
    }
};
template<typename T1,size_t S1,typename T2,size_t S2>
bool operator!=(const AreanaAllocator<T1,S1>&,const AreanaAllocator<T2,S2>&){
    return false;
}
int main (){
    std::vector<int,ArenaAllocator<int,1024>>vec;
    std::cout << 
}
