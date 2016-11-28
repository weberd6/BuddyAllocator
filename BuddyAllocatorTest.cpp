#include "BuddyAllocator.hpp"

#include <iostream>

int main() {

    std::vector<char*> allocations_;

    BuddyAllocator ba;
    ba.initialize(30, 4);

    char* alloc = nullptr;
    for (int i = 29; i > 2; i--) {
        alloc = (char*)ba.allocate(i);
        allocations_.push_back(alloc);
    }

    for (auto a : allocations_) {
        ba.deallocate(a);
        //ba.printFreeLists();
    }

    return 0;
}
