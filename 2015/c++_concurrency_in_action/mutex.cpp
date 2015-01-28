#include <climits>
#include <cstdint>
#include <exception>
#include <mutex>

class hierarchal_mutex
{
public:
    explicit hierarchal_mutex(uint64_t val_) :
        val(val_),
        prev(0)
    {
    }

    void lock()
    {
        check();
        mu.lock();
        update();
    }

    void unlock()
    {
        thread_val = prev;
        mu.unlock();
    }

    bool try_lock()
    {
        check();
        if (!mu.try_lock())
            return false;
        update();
        return true;
    }

private:
    void check()
    {
        if (thread_val <= val)
            throw std::logic_error("mutex hiearchy violated");
    }

    void update()
    {
        prev = thread_val;
        thread_val = val;
    }

    std::mutex mu;
    uint64_t val;                            // hierarchy value
    uint64_t prev;                           // previous hierarchy
    static thread_local uint64_t thread_val; // this thread hierarchy val
};

thread_local uint64_t hierarchal_mutex::thread_val(ULONG_MAX);

int main()
{
}
