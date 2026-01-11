//
// Created by khaled-sawaid on 11/01/2026.
//

#ifndef DS_WET2_WINTER_2026_01_AVLTREE_H
#define DS_WET2_WINTER_2026_01_AVLTREE_H

// Simple, robust AVL Tree with subtree-size (order statistics) + iterative clear().
// No STL containers.

template <typename Key>
struct DefaultLess {
    bool operator()(const Key& a, const Key& b) const { return a < b; }
};

template <typename Key, typename Value, typename Less = DefaultLess<Key>>
class AVLTree {
public:
    struct Node {
        Key key;
        Value value;
        int height;
        int subSize;   // subtree size for select(k)
        Node* left;
        Node* right;

        Node(const Key& k, const Value& v)
            : key(k), value(v), height(1), subSize(1), left(nullptr), right(nullptr) {}
    };

private:
    Node* root;
    Less less;

private:
    static int h(Node* n) { return n ? n->height : 0; }
    static int sz(Node* n) { return n ? n->subSize : 0; }
    static int max2(int a, int b) { return (a > b) ? a : b; }

    static void recalc(Node* n) {
        if (!n) return;
        n->height  = 1 + max2(h(n->left), h(n->right));
        n->subSize = 1 + sz(n->left) + sz(n->right);
    }

    static int balanceFactor(Node* n) {
        return n ? (h(n->left) - h(n->right)) : 0;
    }

    static Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* t2 = x->right;

        x->right = y;
        y->left = t2;

        recalc(y);
        recalc(x);
        return x;
    }

    static Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* t2 = y->left;

        y->left = x;
        x->right = t2;

        recalc(x);
        recalc(y);
        return y;
    }

    Node* rebalance(Node* n) {
        if (!n) return n;

        recalc(n);
        int bf = balanceFactor(n);

        // Left heavy
        if (bf > 1) {
            if (balanceFactor(n->left) < 0) {
                n->left = rotateLeft(n->left);
            }
            return rotateRight(n);
        }

        // Right heavy
        if (bf < -1) {
            if (balanceFactor(n->right) > 0) {
                n->right = rotateRight(n->right);
            }
            return rotateLeft(n);
        }

        return n;
    }

    Node* insertRec(Node* n, const Key& key, const Value& value, bool& inserted) {
        if (!n) {
            inserted = true;
            return new Node(key, value);
        }

        if (less(key, n->key)) {
            n->left = insertRec(n->left, key, value, inserted);
        } else if (less(n->key, key)) {
            n->right = insertRec(n->right, key, value, inserted);
        } else {
            inserted = false; // key exists
            return n;
        }

        return rebalance(n);
    }

    static Node* minNode(Node* n) {
        Node* cur = n;
        while (cur && cur->left) cur = cur->left;
        return cur;
    }

    Node* removeRec(Node* n, const Key& key, bool& removed) {
        if (!n) {
            removed = false;
            return nullptr;
        }

        if (less(key, n->key)) {
            n->left = removeRec(n->left, key, removed);
        } else if (less(n->key, key)) {
            n->right = removeRec(n->right, key, removed);
        } else {
            // found
            removed = true;

            // 0 or 1 child
            if (!n->left || !n->right) {
                Node* child = n->left ? n->left : n->right;
                delete n;
                return child;
            }

            // 2 children: replace with successor key/value
            Node* succ = minNode(n->right);
            n->key = succ->key;
            n->value = succ->value;

            bool dummy = false;
            n->right = removeRec(n->right, succ->key, dummy);
        }

        return rebalance(n);
    }

    Node* findRec(Node* n, const Key& key) const {
        Node* cur = n;
        while (cur) {
            if (less(key, cur->key)) {
                cur = cur->left;
            } else if (less(cur->key, key)) {
                cur = cur->right;
            } else {
                return cur;
            }
        }
        return nullptr;
    }

    // Iterative destroy with no STL and no recursion.
    // Repeatedly rotate left child up until no left, then delete and go right.
    static void destroyIterative(Node* n) {
        while (n) {
            if (n->left) {
                Node* l = n->left;
                n->left = l->right;
                l->right = n;
                n = l;
            } else {
                Node* r = n->right;
                delete n;
                n = r;
            }
        }
    }

public:
    AVLTree() : root(nullptr), less(Less()) {}
    ~AVLTree() { clear(); }

    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    void clear() {
        destroyIterative(root);
        root = nullptr;
    }

    int size() const { return sz(root); }
    bool isEmpty() const { return root == nullptr; }

    // returns false if key already exists
    bool insert(const Key& key, const Value& value) {
        bool inserted = false;
        root = insertRec(root, key, value, inserted);
        return inserted;
    }

    // returns false if key didn't exist
    bool remove(const Key& key) {
        bool removed = false;
        root = removeRec(root, key, removed);
        return removed;
    }

    // returns pointer to value or nullptr
    Value* find(const Key& key) {
        Node* n = findRec(root, key);
        return n ? &n->value : nullptr;
    }

    const Value* find(const Key& key) const {
        Node* n = findRec(root, key);
        return n ? &n->value : nullptr;
    }

    // 1-indexed in-order select. nullptr if out of range.
    const Node* select(int k) const {
        if (k <= 0 || k > size()) return nullptr;

        Node* cur = root;
        while (cur) {
            int leftSize = sz(cur->left);
            if (k == leftSize + 1) return cur;

            if (k <= leftSize) {
                cur = cur->left;
            } else {
                k -= (leftSize + 1);
                cur = cur->right;
            }
        }
        return nullptr;
    }
};

#endif //DS_WET2_WINTER_2026_01_AVLTREE_H
