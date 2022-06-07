
#include "reserve_allocator.h"
#include <iostream>
#include <list>

int main() {

    auto m = std::list<int, LinearAllocator<int,1000>>();
    std::cout << m.max_size() << std::endl;
	for (int i = 0; i < 61; ++i)
    {
        m.push_back(i);
    }

    for (auto value : m)
    {
        std::cout << value <<  std::endl;
    }

    return 0;
}
