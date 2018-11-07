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

    enum class Alignment : size_t
    {
        align1 = 0x1 << 0,
        align2 = 0x1 << 1,
        align4 = 0x1 << 2,
        align8 = 0x1 << 3,
        align16 = 0x1 << 4,
        align32 = 0x1 << 5,
        align64 = 0x1 << 6,
        align128 = 0x1 << 7,
        align256 = 0x1 << 8,
        align512 = 0x1 << 9
    };
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

        size_t allocated_size_max = 0;

        for (uint32_t i = 0; i < 10; ++i)
        {
            const size_t size = rand() % 0x100000;
            const size_t align = 1 << (rand() % 10);

            allocated_size_max += size + (align - 1);

            if (allocated_size_max >= kMemorySize)
            {
                VPRINTF("might not have enough memory capacity. (%zd)\n", allocated_size_max);
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

        uintptr_t prev = (uintptr_t)base_;
        size_t allocated_size = 0;

        for (uint32_t i = 0; i < 100; ++i)
        {
            const size_t size = rand() % (16 * 1000 * 1000);
            const size_t alignment = (size_t)Alignment::align1;

            allocated_size += size; // no need to care about alignment since with Alignment::align1

            if (allocated_size >= kMemorySize)
            {
                VPRINTF("do not have enough memory capacity. (%zd)\n", allocated_size);
                break;
            }

            void* const alloc_addr = allocator_->alloc(size, alignment);

            {
                EXPECT_EQ((uintptr_t)alloc_addr, prev);
                EXPECT_TRUE(((uintptr_t)alloc_addr % alignment) == 0);
                EXPECT_EQ((uintptr_t)allocator_->getCurrent(), prev + size);
                EXPECT_EQ(allocator_->getSizeInBytes(), allocated_size);
                EXPECT_EQ(allocator_->getRemainingSizeInBytes(), kMemorySize - allocated_size);
            }

            prev = (uintptr_t)allocator_->getCurrent();
        }

        allocator_->reset();
        prev = (uintptr_t)base_;
        allocated_size = 0;

        EXPECT_EQ(base_, allocator_->getStart());
        EXPECT_EQ(base_, allocator_->getCurrent());

        for (uint32_t i = 0; i < 100; ++i)
        {
            const size_t size = rand() % (16 * 1000 * 1000);
            const size_t alignment = (size_t)Alignment::align4;

            if (allocated_size + size >= kMemorySize)
            {
                VPRINTF("do not have enough memory capacity. (%zd)\n", allocated_size + size);
                break;
            }

            void* const alloc_addr = allocator_->alloc(size, alignment);

            {
                EXPECT_GE((uintptr_t)alloc_addr, prev);
                EXPECT_LT((uintptr_t)alloc_addr, prev + alignment);
                EXPECT_TRUE(((uintptr_t)alloc_addr % alignment) == 0);

                EXPECT_GE((uintptr_t)allocator_->getCurrent(), prev + size);
                EXPECT_LT((uintptr_t)allocator_->getCurrent(), prev + size + alignment);
                EXPECT_GE(allocator_->getSizeInBytes(), allocated_size + size);
                EXPECT_LT(allocator_->getSizeInBytes(), allocated_size + size + alignment);
                EXPECT_GT(allocator_->getRemainingSizeInBytes(), kMemorySize - allocated_size - size - alignment);
                EXPECT_LE(allocator_->getRemainingSizeInBytes(), kMemorySize - allocated_size - size);
            }

            prev = (uintptr_t)allocator_->getCurrent();
            allocated_size = (uintptr_t)allocator_->getCurrent() - (uintptr_t)base_;
        }

        // Todo: test alignment 16

        // Todo: test alignment random
    }

} // namespace anonymouse


int test_main(int argc, char * argv[])
{
    srand(0);

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
