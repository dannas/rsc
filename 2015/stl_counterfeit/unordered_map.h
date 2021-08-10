#pragma once
namespace danstd {

template <typename T>
class unordered_map {
public:
    unordered_map()
        : buf_(new T[64]), cap_(64), size_(0) {
    }
    size_t size() {
        return size_;
    }
private:
    T *buf_;
    size_t cap_;
    size_t size_;
};

}
