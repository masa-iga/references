#include <cstdio>
#include "memory_allocator.h"
#include "gtest/gtest.h"

#define VPRINTF(...) \
do { \
fprintf(stdout, "%s(L%d): ", __func__, __LINE__); \
fprintf(stdout, __VA_ARGS__); \
} while (false)

namespace { // for constants
    const size_t kMemorySize = 1000 * 1000 * 1000;
} // namespace anonymouse

namespace { // for test fixture
    class AllocatorTest : public ::testing::Test
    {
    public:
        void SetUp();
        void TearDown();

        Mif::StackAllocator* allocator_;
        void* base_;
    };

    void AllocatorTest::SetUp()
    {
        base_ = malloc(kMemorySize);
        allocator_ = new Mif::StackAllocator(base_, kMemorySize);
    }

    void AllocatorTest::TearDown()
    {
        delete(allocator_);
        free(base_);
    }
} // namespace anonymouse


namespace { // for functions

    TEST_F(AllocatorTest, startEnd)
    {
        VPRINTF("base address 0x%p\n", base_);

        const void* const exp_end = (void*)((uintptr_t)base_ + kMemorySize);

        {
            EXPECT_EQ(allocator_->getStart(), base_);
            EXPECT_EQ(allocator_->getCurrent(), base_);
            EXPECT_EQ(allocator_->getEnd(), exp_end);
            EXPECT_EQ(allocator_->getSizeInBytes(), 0);
            EXPECT_EQ(allocator_->getRemainingSizeInBytes(), kMemorySize);
        }

        size_t allocated_size_worst = 0;

        for (uint32_t i = 0; i < 10; ++i)
        {
            const size_t size = rand() % 0x100000;
            const size_t align = 1 << (rand() % 9);

            allocated_size_worst += size + (align - 1);
            if (allocated_size_worst >= kMemorySize)
            {
                VPRINTF("might not have enough memory capacity. (%zd)\n", allocated_size_worst);
                break;
            }

            allocator_->alloc(size, align);

            EXPECT_EQ(allocator_->getStart(), base_);
            EXPECT_EQ(allocator_->getEnd(), exp_end);
        }

        allocator_->reset();

        {
            EXPECT_EQ(allocator_->getStart(), base_);
            EXPECT_EQ(allocator_->getCurrent(), base_);
            EXPECT_EQ(allocator_->getEnd(), exp_end);
            EXPECT_EQ(allocator_->getSizeInBytes(), 0);
            EXPECT_EQ(allocator_->getRemainingSizeInBytes(), kMemorySize);
        }
    }


    TEST_F(AllocatorTest, sizeAlign)
    {
        VPRINTF("base address 0x%p\n", base_);
        uintptr_t prev = reinterpret_cast<uintptr_t>(base_);
        size_t allocated_size = 0;

        for (uint32_t i = 0; i < 100; ++i)
        {
            const size_t size = rand() % (16 * 1000 * 1000);
            const size_t align = 0x1;

            allocated_size += size; // no need to care about alignment since align = 1
            if (allocated_size >= kMemorySize)
            {
                VPRINTF("do not have enough memory capacity. (%zd)\n", allocated_size);
                break;
            }

            void* const alloc_addr = allocator_->alloc(size, align);

            const uintptr_t alloc_size_as_uintptr_t = reinterpret_cast<uintptr_t>(alloc_addr) - prev;
            const size_t alloc_size = static_cast<size_t>(alloc_size_as_uintptr_t);
            const size_t alloc_align = reinterpret_cast<size_t>(alloc_addr) % align;

            EXPECT_GE(alloc_size, size);
            EXPECT_LE(alloc_size, size + align - 1);
            EXPECT_TRUE(alloc_align == 0);
            EXPECT_EQ(allocator_->getSizeInBytes(), allocated_size);
            EXPECT_EQ(allocator_->getRemainingSizeInBytes(), kMemorySize - allocated_size);

            prev = reinterpret_cast<uintptr_t>(alloc_addr);
        }

        allocator_->reset();

        EXPECT_EQ(base_, allocator_->getCurrent());
        EXPECT_EQ(allocator_->getStart(), allocator_->getCurrent());
    }

    // Todo: test another functions

} // namespace anonymouse


int test_main(int argc, char * argv[])
{
    srand(0);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
