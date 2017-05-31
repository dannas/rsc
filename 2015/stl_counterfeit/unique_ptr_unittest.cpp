
#include "unique_ptr.h"

#include <gtest/gtest.h>

using namespace danstd;

struct Calls {
    Calls() : ctor(0), copyCtor(0), dtor(0) {}
    int ctor;
    int copyCtor;
    int dtor;
};

class AllocationCounter {
public:
    AllocationCounter(Calls *calls)
        : calls_(calls) {
        calls_->ctor++;
    }

    AllocationCounter(const AllocationCounter& rhs) {
        calls_ = rhs.calls_;
        calls_->copyCtor++;
    }

    ~AllocationCounter() {
        calls_->dtor++;
    }

private:
    Calls *calls_;
};

TEST(UniquePtr, construct_ConstructAndFree) {
    Calls calls;
    {
        auto ptr = make_unique<AllocationCounter>(&calls);
        ASSERT_EQ(1, calls.ctor);
        ASSERT_EQ(0, calls.copyCtor);
        ASSERT_EQ(0, calls.dtor);
        ASSERT_TRUE(ptr);
        ASSERT_TRUE(ptr.get());
    }
    ASSERT_EQ(1, calls.ctor);
    ASSERT_EQ(0, calls.copyCtor);
    ASSERT_EQ(1, calls.dtor);
}

TEST(UniquePtr, construct_TakeOwnership) {
    Calls calls;
    AllocationCounter *c = new AllocationCounter(&calls);
    {
        unique_ptr<AllocationCounter> ptr(c);
        ASSERT_EQ(1, calls.ctor);
        ASSERT_EQ(0, calls.copyCtor);
        ASSERT_EQ(0, calls.dtor);
        ASSERT_TRUE(ptr);
        ASSERT_TRUE(ptr.get());
    }
    ASSERT_EQ(1, calls.ctor);
    ASSERT_EQ(0, calls.copyCtor);
    ASSERT_EQ(1, calls.dtor);
}

TEST(UniquePtr, construct_TransferOwnership) {
    Calls calls;
    {
        auto ptr1 = make_unique<AllocationCounter>(&calls);
        ASSERT_EQ(1, calls.ctor);
        ASSERT_EQ(0, calls.copyCtor);
        ASSERT_EQ(0, calls.dtor);
        ASSERT_TRUE(ptr1);
        ASSERT_TRUE(ptr1.get());

        auto ptr2(std::move(ptr1));
        ASSERT_EQ(1, calls.ctor);
        ASSERT_EQ(0, calls.copyCtor);
        ASSERT_EQ(0, calls.dtor);
        ASSERT_FALSE(ptr1);
        ASSERT_FALSE(ptr1.get());
        ASSERT_TRUE(ptr2);
        ASSERT_TRUE(ptr2.get());
    }
    ASSERT_EQ(1, calls.ctor);
    ASSERT_EQ(0, calls.copyCtor);
    ASSERT_EQ(1, calls.dtor);

}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
