#pragma once

#include <utility>
#include <iterator>
#include <iostream> // TODO(dannas): Remove after testing

namespace danstd {

// A bare minimum symboltable for key/value lookups. Do NOT use in production;
// for educational purpose only.
//
// It demonstrates the most commonly used map functionality such as key/value
// lookup, insertion, deletion and iteration.
//
// The map is built upon a plain, non-balancing, binary search tree (BST). So
// worse case runtime is O(n).
//
// The class does not provide allocators, nor custom Compare functions.
template <typename K, typename V>
class map {

    // The internal node that constructs the tree.
    struct node {
        node()
            : left(nullptr)
            , right(nullptr)
            , parent(nullptr)
            , kv() {
        }
        node(node* l, node* r, node* p, const K& key, const V& value)
            : left(l)
            , right(r)
            , parent(p)
            , kv(key, value) {
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
            : node_(n) {
        }

        iterator& operator++() {
            if (!node_) {
                return *this;
            }
            // Either we have a right subtree beneath or we need to go up until we're
            // no longer part of a right subtree.
            if (node_->right) {
                node_ = node_->right;
                while (node_->left)
                    node_ = node_->left;
            } else {
                node* parent = node_->parent;
                while (parent && node_ == parent->right) {
                    node_ = parent;
                    parent = node_->parent;
                }
                node_ = parent;
            }
            return *this;
        }

        iterator operator++(int) {
            iterator temp(*this);
            operator++();
            return temp;
        }

        friend bool operator==(const iterator& lhs, const iterator& rhs) {
            return lhs.node_ == rhs.node_;
        }

        friend bool operator!=(const iterator& lhs, const iterator& rhs) {
            return !(lhs == rhs);
        }

        value_type& operator*() {
            return node_->kv;
        }

        value_type* operator->() {
            return &node_->kv;
        }

    private:
        node* node_;
    };

    map()
        : root_(nullptr) {

    }

    // TODO(dannas): Make |other| const and const overload the neccessary member functions
    map(map& other) 
        : root_(nullptr) {
        for (auto& kv : other)
            root_ = put(nullptr, root_, kv.first, kv.second);
    }

    ~map() {
        deleteTree(root_);
        delete root_;
    }

    map& operator=(map other) {
        swap(other);
        return *this;
    }

    void swap(map& other) noexcept {
        using std::swap;
        swap(root_, other.root_);
    }

    V& operator[](const K& key) {
        // TODO(dannas): Rewrite to only traverse the tree once.
        V* val = get(root_, key);
        if (!val) {
            V def = V();
            root_ = put(nullptr, root_, key, def);
        }
        return *get(root_, key);
    }

    size_t size() {
        return size(root_);
    }

    iterator begin() {
        node* n = root_;
        while (n && n->left)
            n = n->left;
        return iterator(n);
    }

    iterator end() {
        return iterator(nullptr);
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

    node* put(node* parent, node* n, const K& key, const V& val) {
        if (!n)
            n = new node(nullptr, nullptr, parent, key, val);

        if (key < n->kv.first)
            n->left = put(n, n->left, key, val);
        else if (key > n->kv.first)
            n->right = put(n, n->right, key, val);
        else
            n->kv.second = val;
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

// Specialize std::swap.
//
// We can't do a proper template specialization for template types since C++
// does  not allow partial specialization for classes.
// We can't define this function inside the std namespace since only total template
// specializations are allowed in there.
//
// See Effective C++ Item 25. Consider  support for non-throwing swap.
template <typename K, typename V>
void swap(map<K, V>& lhs, map<K, V>& rhs) noexcept {
    lhs.swap(rhs);
}
}
