#pragma once
#include <utility>

namespace danstd {

template <typename K, typename V>
class map {
    struct node {
        node() : left(nullptr), right(nullptr), kv() {}
        node(node* l, node* r, const K& key, const V& value) 
            : left(l), right(r), kv(key, value) {
        }

        node* left;
        node* right;
        std::pair<K, V> kv;
    };

public:
    map() : root_(nullptr) {

    }

    V& operator[](const K& key) {
        // TODO(dannas): Rewrite to only traverse the tree once.
        V* val = get(root_, key);
        if (!val) {
            V def = V();
            root_ = put(root_, key, def);
        }
        return *get(root_, key);
    }

    ~map() {
        deleteTree(root_);
        delete root_;
    }
private:
    // TODO(dannas): Can this be simplified?
    void deleteTree(node* n) {
        if (!n)
            return;
        if (n->left) {
            deleteTree(n->left);
            delete n->left;
        }
        if (n->right) {
            deleteTree(n->right);
            delete n->right;
        }
    }

    V* get(node* n, const K& key) {
        if (!n)
            return nullptr;
        if (key < n->kv.first)
            return get(n->left, key);
        else if (key > n->kv.first)
            return get(n->right, key);
        else
            return &n->kv.second;
    }

    node* put(node* n, const K& key, const V& val) {
        if (!n)
            n = new node(nullptr, nullptr, key, val);

        if (key < n->kv.first)
            n->left = put(n->left, key, val);
        else if (key > n->kv.first)
            n->right = put(n->right, key, val);
        else
            n = new node(n->left, n->right, key, val);
        return n;
    }

    node* root_;
};
}
