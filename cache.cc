#include <cassert>
#include <iostream>

#include "cache.hpp"

// slow get page imitation
int slow_get_page_int(int key) 
{ 
    return key;
}

int main() 
{
    int hits = 0;
    int number = 0;
    size_t size_cache = 0;

    std::cin >> size_cache >> number;
    assert(std::cin.good());
    caches::cache_t<int> cache{size_cache};

    for (int i = 0; i < number; ++i)
    {
        int q = 0;
        std::cin >> q;
        assert(std::cin.good());

        if (cache.lookup_update(q, slow_get_page_int))
        hits += 1;
    }

    std::cout << hits << std::endl;
}
