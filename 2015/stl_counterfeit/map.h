#pragma once

#include <utility>
#include <iterator>
#include <iostream> // TODO(dannas): Remove after testing

namespace danstd {


template <typename K, typename V>
class map {
    struct node {
        node() : left(nullptr), right(nullptr), parent(nullptr), kv() {}
        node(node* l, node* r, node* p, const K& key, const V& value)
            : left(l), right(r), parent(p), kv(key, value) {
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

        iterator(node* n, map* m)
            : node_(n), prev_(n) , map_(m){
        }

        iterator& operator++() {
            if (!node_) {
                std::cout << "no node" << std::endl;
                return *this;
            }
            // Tree invariant: The current node is the leftmost of all unvisited nodes

            // CASE previous node is left
            if (prev_ == node_->left) {
                std::cout << "left" << std::endl;
                if (node_->right) {
                    map_->find_leftmost(node_->right);
                } else {
                    prev_ = node_;
                    node_ = node_->parent;
                }
            // CASE previous node is parent
            } else if (prev_ == node_->parent) {
                std::cout << "right" << std::endl;
                // find grandparent
            } else if (prev_ == node_) {
                std::cout << "Not left or right" << std::endl;
                node_ = node_->parent;
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

        value_type& operator*() {
            return node_->kv;
        }

        value_type* operator->() {
            return &node_->kv;
        }

    private:
        node* node_;
        node* prev_;
        map* map_;
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
            root_ = put(nullptr, root_, key, def);
        }
        return *get(root_, key);
    }

    size_t size() {
        return size(root_);
    }

    iterator begin() {
        node* n = find_leftmost(root_);
        return iterator(n, this);
    }

    iterator end() {
        return iterator(nullptr, this);
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

    node* find_leftmost(node* n) {
        while (n && n->left)
            n = n->left;
        return n;
    }

    node* find_right_branch(node* n) {
        // What precondition?
        // What is the invariant?
        // What postcondition?
        while (true) {
            if (!n)
                return n;
            if (n->right)
                return n->right;
            n = n->parent;
        }
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
