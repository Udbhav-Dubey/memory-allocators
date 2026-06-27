#include "slab.h"
int main(){
    slabAllocator p(64);
    auto a=p.allocate(8);
//    p.print_debug();
    auto b=p.allocate(8);
    p.debug();
    p.free(a);
    p.debug();
    auto c=p.allocate(8);
    p.debug();
//    p.pfree(a+1);
}
