#include <smart_pointers/shared_ptr/shared_ptr.hpp>

#include <gtest/gtest.h>

namespace
{
    struct A {};

    TEST(SharedPointer, SimpleUseCount)
    {
        lvn::shared_ptr<A> sp(new A);
        EXPECT_EQ(1, sp.use_count());

        sp.reset();
        EXPECT_EQ(0, sp.use_count());

        sp.reset(new A);
        EXPECT_EQ(1, sp.use_count());
    }

    TEST(SharedPointer, ComplexUseCount)
    {
        lvn::shared_ptr<A> sp1(new A);
        lvn::shared_ptr<A> sp2 = sp1;

        EXPECT_TRUE(sp1.use_count() == sp2.use_count() && sp1.use_count() == 2);

        sp2.reset();
        EXPECT_TRUE(sp2.use_count() == 0 && sp1.use_count() == 1);
    }

}
