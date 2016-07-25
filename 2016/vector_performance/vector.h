#pragma once

#include <vector>
#include <cstdint>

template <typename T>
class pod_vector {
public:
    pod_vector(T N) : a_(new T[N]) {}
    ~pod_vector() { delete[] a_; }

    T& operator[](int i) { return a_[i]; }
    T* data() { return a_; }
private:
    T* a_;
};
