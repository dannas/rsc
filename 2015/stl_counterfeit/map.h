#pragma once

#include <utility>
#include <iterator>

namespace danstd {

template <typename K, typename V>
class map {
    struct node {
        node() : left(nullptr), right(nullptr), parent(nullptr), kv() {}
        node(node* l, node* r, const K& key, const V& value) 
            : left(l), right(r), kv(key, value) {
        }

        node* left;
        node* right;
        node* parent;
        std::pair<K, V> kv;
    };

public:

    class iterator {
    public:
        using value_type = std::pair<K, V>;
        using reference = node&;
        using pointer = node*;
        using iterator_category = std::bidirectional_iterator_tag;

        iterator(node* n)
            : node_(n), prev_(nullptr) {
        }

        iterator& operator++() {
            if (!node_)
                return *this;
            K key = node_->kv.first;

            while (key <= node_->kv.first) {
                if (node_->parent == prev_) {
                    prev_ = node_;
                    node_ = node_->right;
                } else if (node_->left == prev_) {
                    prev_ = node_;
                    node_ = node_->left;
                } else if (node_->right == prev_) {
                    prev_ = node_;
                    node_ = node_->parent;
                }
            }
            return *this;
        }

        iterator operator++(int) {
            iterator temp(*this);
            operator++();
            return temp;
        }

        value_type operator*() {
            return node_->kv;
        }

    private:
        node* node_;
        node* prev_;
    };

    map() : root_(nullptr) {

    }
    // TODO(dannas): Add copy constructor etcetera
    ~map() {
        deleteTree(root_);
        delete root_;
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

    size_t size() {
        return size(root_);
    }

    iterator begin() {
        return iterator(root_);
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

    size_t size(node* node) {
        if (!node)
            return 0;

        size_t N = 0;
        N += size(node->left);
        N += size(node->right);
        return N + 1;
    }

    node* root_;
};
}
