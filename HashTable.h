//
// Created by khaled-sawaid on 11/01/2026.
//

#ifndef DS_WET2_WINTER_2026_01_HASHTABLE_H
#define DS_WET2_WINTER_2026_01_HASHTABLE_H

#include <new> // std::bad_alloc (optional to catch in your code)

template <typename Key, typename Value>
class HashTable {
private:
    struct Node {
        Key key;
        Value value;
        Node* next;

        Node(const Key& k, const Value& v, Node* n)
            : key(k), value(v), next(n) {}
    };

    Node** buckets;      // array of heads
    int capacity;        // number of buckets
    int count;           // number of stored elements

private:
    static unsigned int hashInt(unsigned int x) {
        // A decent integer mixer (works fine for IDs)
        x ^= x >> 16;
        x *= 0x7feb352dU;
        x ^= x >> 15;
        x *= 0x846ca68bU;
        x ^= x >> 16;
        return x;
    }

    static int nextPrime(int minVal) {
        // Fixed prime table (enough for wet constraints). Grows roughly x2.
        static const int primes[] = {
            17, 37, 79, 163, 331, 673, 1361, 2729, 5471, 10949, 21911, 43853,
            87719, 175447, 350899, 701819, 1403641, 2807303, 5614657, 11229331,
            22458671, 44917381, 89834777, 179669557
        };
        static const int P = (int)(sizeof(primes) / sizeof(primes[0]));

        for (int i = 0; i < P; i++) {
            if (primes[i] >= minVal) return primes[i];
        }
        // If exceeded (very unlikely in wet), just return the last one.
        return primes[P - 1];
    }

    int indexOfKey(const Key& key) const {
        // Here Key is expected to be int in our wet usage.
        unsigned int x = (unsigned int)key;
        unsigned int h = hashInt(x);
        return (int)(h % (unsigned int)capacity);
    }

    void freeBuckets(Node** arr, int cap) {
        if (!arr) return;
        for (int i = 0; i < cap; i++) {
            Node* cur = arr[i];
            while (cur) {
                Node* nxt = cur->next;
                delete cur;
                cur = nxt;
            }
        }
        delete[] arr;
    }

    void rehash(int newCap) {
        Node** newBuckets = new Node*[newCap];
        for (int i = 0; i < newCap; i++) newBuckets[i] = nullptr;

        // move nodes (recreate nodes; simplest + safest)
        for (int i = 0; i < capacity; i++) {
            Node* cur = buckets[i];
            while (cur) {
                Node* nxt = cur->next;

                int idx = (int)(hashInt((unsigned int)cur->key) % (unsigned int)newCap);
                newBuckets[idx] = new Node(cur->key, cur->value, newBuckets[idx]);

                delete cur;
                cur = nxt;
            }
            buckets[i] = nullptr;
        }

        delete[] buckets;
        buckets = newBuckets;
        capacity = newCap;
        // count stays the same
    }

    void maybeGrow() {
        // load factor threshold ~ 0.75
        if (count * 4 < capacity * 3) return;
        int newCap = nextPrime(capacity * 2);
        rehash(newCap);
    }

public:
    HashTable()
        : buckets(nullptr), capacity(0), count(0)
    {
        capacity = nextPrime(17);
        buckets = new Node*[capacity];
        for (int i = 0; i < capacity; i++) buckets[i] = nullptr;
    }

    ~HashTable() {
        clear();
    }

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;

    void clear() {
        freeBuckets(buckets, capacity);
        buckets = nullptr;
        capacity = 0;
        count = 0;
    }

    int size() const { return count; }
    bool isEmpty() const { return count == 0; }

    // returns pointer to stored Value, or nullptr if not found
    Value* find(const Key& key) {
        if (!buckets) return nullptr;
        int idx = indexOfKey(key);
        Node* cur = buckets[idx];
        while (cur) {
            if (cur->key == key) return &cur->value;
            cur = cur->next;
        }
        return nullptr;
    }

    const Value* find(const Key& key) const {
        if (!buckets) return nullptr;
        int idx = indexOfKey(key);
        Node* cur = buckets[idx];
        while (cur) {
            if (cur->key == key) return &cur->value;
            cur = cur->next;
        }
        return nullptr;
    }

    // returns false if key already exists
    bool insert(const Key& key, const Value& value) {
        if (!buckets) return false;

        int idx = indexOfKey(key);
        Node* cur = buckets[idx];
        while (cur) {
            if (cur->key == key) return false;
            cur = cur->next;
        }

        buckets[idx] = new Node(key, value, buckets[idx]);
        count += 1;

        maybeGrow();
        return true;
    }
};

#endif // DS_WET2_WINTER_2026_01_HASHTABLE_H
