//Concurrent HashMap implementation
/*
Memory reclamation : When removing data from the structure, how can we know that no reader is currently accessing that Data. 
Releasing prematurely can lead to access violation and possible SIGSEGVs 
shared_ptr is one 
*/
#include <shared_mutex>
#include "HashNode.h"
namespace ThreadSafe
{
template <typename K, typename V, typename F = std::hash<K>>
class ConcurrentHashMap
{
public:
    ConcurrentHashMap(size_t hashSize_ = MAP_SIZE_DEFAULT) : hashMapSize(hashSize_)
    {
        hashMap = new HashBucket<K, V>[hashSize];
    }
    ~ConcurrentHashMap()
    {
        delete[] hashMap;
    }

    //Copy and Move are not supported for now
    ConcurrentHashMap(const ConcurrentHashMap &) = delete;
    ConcurrentHashMap(ConcurrentHashMap &&) = delete;
    ConcurrentHashMap &operator=(const ConcurrentHashMap &) = delete;
    ConcurrentHashMap &operator=(ConcurrentHashMap &&) = delete;

    // Find operation on the HashMap
    bool find(const K &key, V &value)
    {
        size_t hashValue = hashFn(key) % hashMapSize;
        return hashMap[hashValue].find(key, value);
    }

    void insert(const K &key, const V &value)
    {
        size_t hashValue = hashFn(key) & hashMapSize;
        hashMap[hashValue].insert(key, value);
    }

    void erase(const K &key)
    {
        size_t hashValue = hashFn(key) % hashMapSize;
        hashMap[hashValue].erase(key);
    }

    void clear()
    {
        for (size_t i = 0; i < hashMapSize; i++)
        {
            (hashMap[i]).clear();
        }
    }

private:
    HashBucket<K, V> *hashMap;
    F hashFn;
    const size_t hashMapSize;
};
} // namespace ThreadSafe