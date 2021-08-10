#pragma once

#include <utility>
#include <iterator>
#include <iostream> // TODO(dannas): Remove after testing

namespace danstd {

// The map class represents an ordered symbol table of generic key-value pairs.
// It supports the usual put, get, contains, delete, size and is-empty methods.
// It also provides methods for iterating over all the key-value pairs.
// A symbol table implements the associative array abstraction: when associating a
// value with a key that is already present in the symbol table, the convention
// is to replace the old value with the new value.
//
// This implementation uses an (unbalanced) binary search tree. It requires that
// the key type implements the less than operator.
//
// The get/put, contains, size and delete all takes linear time in the worst case, if the
// tree becomes unbalanced. The empty operation take constant time.
// Construction takes constant time.

// The class does not provide allocators, nor custom Compare functions. It also omits
// some of the range finding functions such as equal_range, lower_bound and upper_bound.
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
        node* node = get(root_, key);
        if (!node) {
            V def = V();
            root_ = put(nullptr, root_, key, def);
        }
        return get(root_, key)->kv.second;
    }

    size_t erase(const K& key) {
        node* curr = get(root_, key);
        if (!curr)
            return 0;

        // http://www.algolist.net/Data_structures/Binary_search_tree/Removal
        // Three cases
        // i. no children
        // ii. one children
        // iii. two children
        //
        // Need to specialcase the root node!
        // Case i and ii  are easy
        // Case iii can be handled with a trick: Swap values of smallest node in right subtree and current node
        // Remove the what was smallest node in right subtree

        // TODO(dannas): Make special case for parent == root (or is it for curr == root)?
        node* parent = curr->parent;
        //node* parentPtr = parent->right == curr ? parent->right : parent->left;

        // No children
        if (!curr->left && !curr->right) {
            //parentPtr = nullptr;
            delete curr;
        // One children
        } else if (curr->left && !curr->right) {
            //parentPtr = curr->left;
            curr->left->parent = parent;
        } else if (!curr->left && curr->right) {
            //parentPtr = curr->right;
            curr->right->parent = parent;
        // Two children
        } else {
            ;
            //node* n = curr->right;
#if 0
            while (n->left)
                n = n->left;
            curr->kv = n->kv;
            // TODO(dannas): Update n parent pointer and delete n
            node *nParent = n->parent;
            node *nParentPtr = nParent->right == n ? nParent->right : nParent->left;
            nParentPtr = nullptr;
            delete n;
#endif
        }

#if 0
        // If this was the last element in the tree, mark it as empty.
        if (parent == nullptr && curr->left == nullptr && curr->right == nullptr)
            root_ = nullptr;
#endif

        delete curr;

        return 1;
    }

    // Return the number of key-value pairs in this symbol table.
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

    node* get(node* n, const K& key) {
        if (!n)
            return nullptr;
        if (key < n->kv.first)
            return get(n->left, key);
        else if (key > n->kv.first)
            return get(n->right, key);
        else
            return n;
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

    // Root of the BST.
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
