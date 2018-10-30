#include <cstdio>
#include <cstdint>
#include <cassert>
#include "memory_allocator.h"

namespace Mif {

    StackAllocator::StackAllocator(void* start, size_t size)
    : m_start(static_cast<uint32_t*>(start))
    , m_end(static_cast<uint32_t*>(start) + (size / sizeof(uint32_t)))
    , m_current(static_cast<uint32_t*>(start))
    {
        ;
    }


    void* StackAllocator::alloc(size_t size, size_t align)
    {
        size_t aligned_start = reinterpret_cast<size_t>(m_start);

        if (align > 1)
        {
            aligned_start = (aligned_start + (align-1)) & ~(align-1);
        }

        assert(aligned_start < reinterpret_cast<size_t>(m_end) && "exceeds memory capacity\n");

        void* end = reinterpret_cast<void*>(aligned_start + size);

        assert(end < m_end && "exceeds memory capacity\n");

        if (end > m_end)
            return nullptr;

        m_current = end;

        return m_current;
    }

} // namespace Mif
