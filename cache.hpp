
#pragma once

#include <iterator>
#include <list>
#include <unordered_map>
#include <vector>

namespace caches
{
    template <typename T, typename KeyT = int> struct cache_t
    {
        size_t sz_T1_A;
        size_t sz_T1_B;
        const size_t sz_T2_A;
        const size_t sz_T2_B;

        std::list<T> T1_A;
        std::list<T> T1_B;
        std::list<T> T2_A; // ghost list
        std::list<T> T2_B; // ghost list

        using VectHash = typename std::vector<std::unordered_map<KeyT, ListIt>>;
        using VectCach = typename std::vector<std::list<T>>;
        VectHash hashes_ = {hash_T1_A, hash_T1_B, hash_T2_A, hash_T2_B};
        VectCach caches_ = {T1_A, T1_B, T2_A, T2_B};

        using ListIt = typename std::list<T>::iterator;
        using HashIt = typename std::unordered_map<KeyT, ListIt>::iterator;

        std::unordered_map<KeyT, ListIt> hash_T1_A;
        std::unordered_map<KeyT, ListIt> hash_T2_A;
        std::unordered_map<KeyT, ListIt> hash_T1_B;
        std::unordered_map<KeyT, ListIt> hash_T2_B;
        std::unordered_map<KeyT, int> hash_hits;

        cache_t(size_t sz) : sz_T1_A{sz / 2 + sz % 2}, sz_T1_B{sz / 2},
                             sz_T2_A{sz / 2 + sz % 2}, sz_T2_B{sz / 2} {}

        bool full(std::list<T>& list, size_t sz_) const 
        { 
            return (list.size() == sz_); 
        }

        // возвращает итератор на пару ключ-итератор на страницу в хэш-таблице
        HashIt get_page(KeyT key) const
        {
            HashIt hit;
            for(auto it = hashes_.begin(); it != hashes_.end(); ++it)
            {
                hit = (*it).find(key);
                if(hit != (*it).end())
                {
                    return hit;
                }
            }
            return hit;
        }

        //  возвращает итератор на список, где хранится данная страница
        VectCach::iterator get_cache(ListIt page_it) const
        {
            for(auto it = caches_.begin(); it != caches_.end(); ++it)
            {
                for(auto it_cache = (*it).begin(); it_cache != (*it).end(); ++it_cache)
                {
                    if(it_cache == page_it)
                    {
                        return it;
                    }
                }
            }
            return caches_.end();
        }

        template <typename F> bool lookup_update(KeyT key, F slow_get_page)
        {
            auto page_it = get_page(key)->second;
            auto cache_it = get_cache(page_it);

            if (cache_it == T1_A.begin())
            {
                int hits = hash_hits[key];
                if(hits < 2)
                {
                    if (page_it != cache_it)
                    {
                        T1_A.splice(cache_it, T1_A, page_it, std::next(page_it));
                    }
                    return true;
                }
                else
                {
                    if(full(T1_B, sz_T1_B))
                    {
                        // ключ последнего элемента T1_B 
                        KeyT key_tmp = 0;
                        for (HashIt::const_iterator it = hash_T1_B.begin(); it != hash_T1_B.end(); ++it) 
                        {
                            if (it->second == T1_B.back()) 
                            {
                                key_tmp = it->first;
                            }
                        }

                        if(full(T2_B, sz_T2_B))
                        { 
                            hash_T2_B.erase(T2_B.back());
                            T2_B.pop_back();
                        }
                        T2_B.push_front(*T1_B.back());
                        hash_T2_B[key_tmp] = T2_B.begin();

                        hash_T1_B.erase(T1_B.back());
                        T1_B.pop_back();
                    }
                    T1_B.push_front(*page_it);
                    hash_T1_B[key] = T1_B.begin();                        
                    
                    hash_T1_A.erase(page_it);
                    T1_A.erase(page_it);

                    return true;
                }
            }
            else if(cache_it == T2_A.begin())
            {
                ++sz_T1_A;

                if(full(T1_B, sz_T1_B))
                {
                    // ключ последнего элемента T1_B 
                    KeyT key_tmp = 0;
                    for (HashIt::const_iterator it = hash_T1_B.begin(); it != hash_T1_B.end(); ++it) 
                    {
                        if (it->second == T1_B.back()) 
                        {
                            key_tmp = it->first;
                        }
                    }

                    if(full(T2_B, sz_T2_B))
                    { 
                        hash_T2_B.erase(T2_B.back());
                        T2_B.pop_back();
                    }
                    T2_B.push_front(*T1_B.back());
                    hash_T2_B[key_tmp] = T2_B.begin();

                    hash_T1_B.erase(T1_B.back());
                    T1_B.pop_back();                    

                    --sz_T1_B;
                }
                --sz_T1_B;

                T1_A.push_front(*page_it);
                T2_A.erase(page_it);
            }
            else if(cache_it == T1_B.begin)
            {
                if (page_it != cache_it)
                {
                    cache_.splice(cache_it, T1_B, page_it, std::next(page_it));
                }
                return true;
            }
            else if(cache_it == T2_B.begin())
            {

            }
            else
            {
                
            }
        }
    };
} // namespace caches

                    // if (full(cache, sz_T1_A))
                    // {
                    //     hash_T1_A.erase(cache.back());
                    //     cache.pop_back();
                    // }
                    // cache.push_front(slow_get_page(key));
                    // hash_T1_A[key] = cache.begin();