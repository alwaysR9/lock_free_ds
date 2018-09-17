#include <iostream>

#include "lock_free_list.h"

int main() {
    
    Node x(100, 0);
    std::cout << "is mark: " << x.is_mark() << std::endl;    
    
    x.mark();
    std::cout << "after mark: " << x.is_mark() << std::endl;
    std::cout << "get next: " << x.get_next() << std::endl;

    return 0;
}