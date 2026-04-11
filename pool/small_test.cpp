#include "pool.h"
int main(){
    Pool p(64,4);
    auto a=p.allocate(8);
//    p.print_debug();
    auto b=p.allocate(8);
    p.print_debug();
    p.pfree(a);
    p.print_debug();
    auto c=p.allocate(8);
    p.print_debug();
//    p.pfree(a+1);
}
