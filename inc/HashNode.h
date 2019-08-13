// This is a simple implementation of a templated HashNode

namespace ThreadSafe
{
//
template <typename K, typename V>
class HashNode
{
public:
    HashNode() : next(nullptr)
    {
    }

    HashNode(K key_, V value_) : next(nullptr), key(key_), value(value_)
    {
    }

    ~HashNode()
    {
        next = nullptr;
    }

    const K &getKey()
    {
        const
        {
            return key;
        }
    }

    void setValue(V value_)
    {
        value = value_;
    }

    const V &getValue()
    {
        const
        {
            return value;
        }
    }

    HashNode *next; //next Bucket

private:
    K key;   //Hash Key
    V value; // Value of the corresponding key
};

//Class representing a hash bucket. The bucket is implemented as a singly linked list
// A bucket is always constructed with a dummy node

template <typename K, typename V>
class HashBucket
{
public:
    HashBucket() : head(nullptr)
    {
    }

    ~HashBucket() // delete the bucket
    {
        clear();
    }

    /*
        Find the HashNode with the matching key 
        return bool 
    */

    bool find(const K &key, V &value) const
    {
        std::shared_lock<std::shared_time_mutex> lock(mutex_);
        HashNode<K, V> *node = head;

        while (node != nullptr)
        {
            if (node->getKey() == key)
            {
                value = node->getValue();
                return true;
            }

            node = node->next;
        }
        return false;
    }

    void insert(const K &key, const V &value)
    {
        std::unique_lock<std::shared_time_mutex> lock(_mutex);

        HashNode<K, V> *prev = nullptr;
        HashNode<K, V> *node = head;

        while (node != nullptr && node->getKey() != key)
        {
            prev = node;
            node = node->next;
        }

        if (node == nullptr) // Need to create new HashNode in the bucket
        {
            if (head == nullptr)
            {
                head = new HashNode<K, V>(key, value);
            }
            else
            {
                prev->next = new HashNode<K, V>(key, value);
            }
        }
        else
        {
            node->setValue(value); //Key found, update the value
        }
    }

    //Function to remove an entry from the bucket , if found
    void erase(const K &key)
    {
        std::unique_lock<std::shared_time_mutex> lock(mutex_);
        HashNode<K, V> *prev = nullptr;
        HashNode<K, V> *node = head;

        while (node != nullptr && node->getKey() != key)
        {
            prev = node;
            node = node->next;
        }

        if (node == nullptr)
        {
            return;
        }

        else
        {
            if (head == node)
            {
                head = head->next;
            }
            else
            {
                prev->next = node->next;
            }

            delete node; //Free up memory
        }
    }
    void clear()
    {
        std::unique_lock<std::shared_time_mutex> lock(mutex_);
        HashNode<K, V> *prev = nullptr;
        HashNode<K, V> *node = head;

        while (node != nullptr)
        {
            prev = node;
            node = node->next;
            delete prev;
        }
        head = nullptr;
    }

private:
    HashNode<K, V> *head; // Head node of the bucket
    std::shared_time_mutex mutex_;
};
} // namespace ThreadSafe