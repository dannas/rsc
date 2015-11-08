#pragma once

#include <utility>
#include <iostream>
#include <vector>

// A bare minimum smart pointer for exclusive ownership. Modelled on Chromiums
// scoped_ptr and std::unique_ptr. Do NOT use in production; for educational
// purpose only.
//
// The number of implemented operators is kept at a minimum; Use get() to
// access the raw pointer when doing comparisons or pointer arithmetic.
//
// Conversions between unique_ptr<T> and T* are disallowed since they lead to
// subtle errors that are hard to track down.
//
// This class does not provide custom deleters, nor specializations for arrays.
// Both are useful features, but this class is meant to provide only the bare
// essentials.
//
// For brevity, noexcept specifiers have been omitted.
//
// Example usage:
//
//  {
//      auto foo = make_unique<Foo>("wee");
//      // foo goes out of scope, releasing the pointer with it.
//  }
//
//  {
//      unique_ptr<Foo> foo;          // No pointer managed
//
//      foo.reset(new Foo("wee"));    // Now a pointer is managed.
//      foo.reset(new Foo("wee2"));   // Foo("wee") was destroyed.
//      foo.reset(new Foo("wee3"));   // Foo("wee2") was destroyed.
//      foo->method();                // Foo::Method() called.
//      foo.get()->method();          // Foo::Method() called.
//      SomeFunc(foo.release());      // SomeFunc takes ownership, foo no longer
//                                    // manages a pointer.
//      foo.reset(new Foo("wee4"));   // foo manages a pointer again.
//      foo.reset();                  // Foo("wee4") destroyed, foo no longer
//                                   // manages a pointer.
//  }  // foo wasn't managing a pointer, so nothing was destroyed.
//
// SEE ALSO:
//      std::unique_ptr     (http://en.cppreference.com/w/cpp/memory/unique_ptr)
//      base::scoped_ptr    (https://chromium.googlesource.com/chromium/src/base/+/master/memory/scoped_ptr.h)
//
template <typename T>
class unique_ptr {
public:
    // Constructs a uníque_ptr with |p| as the pointer to the managed object. T
    // must be a complete type and |p| must be convertible to T*.
    explicit unique_ptr(T *p = nullptr) : ptr(p) { }

    // If get() == nullptr, there are no effects; otherwise the owned object is
    // destroyed.
    ~unique_ptr()                           { delete ptr; }

    // Construct a unique_ptr by transferring ownership from |other| to *this.
    unique_ptr(unique_ptr&& other)          { ptr = other.release(); }

    // Transfer ownership from |other| to *this.
    unique_ptr& operator=(unique_ptr&& other) {
        reset(other.release());
        return *this;
    }

    // Disallow copy and assign.
    unique_ptr(const unique_ptr& other) = delete;
    unique_ptr& operator=(const unique_ptr& other) = delete;

    // Replaces the managed object.
    void reset(T* p) {
        delete ptr;
        ptr = p;
    }

    // Returns a pointer to the managed object and releases ownership.
    T* release() {
        T* old_ptr = ptr;
        ptr = nullptr;
        return old_ptr;
    }

    // Returns pointer to the managed object.
    T* get()                       { return  ptr; }

    // Dereferences pointer to the managed object; undefined behaviour if 
    // get() == nullptr.
    T& operator*()                 { return *ptr; }
    T* operator->()                { return  ptr; }

    // Checks whether *this owns an object, i.e. whether get() != nullptr.
    explicit operator bool() const { return  ptr; }

private:
    T* ptr;
};

// Constructs an object of type T and wraps it in a unique_ptr. The arguments
// |args| are passed to the constructor of T.
template <typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

