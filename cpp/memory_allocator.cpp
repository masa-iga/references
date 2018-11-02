#include <cstdio>
#include <cstdint>
#include <cassert>
#include "memory_allocator.h"

namespace Mif {

    StackAllocator::StackAllocator(void* start, size_t size)
    : m_start(reinterpret_cast<uintptr_t>(start))
    , m_end(reinterpret_cast<uintptr_t>(start) + size)
    , m_current(reinterpret_cast<uintptr_t>(start))
    {
        ;
    }


    // ToDo: refine alignment
    void* StackAllocator::alloc(size_t size, size_t align)
    {
        uintptr_t aligned_start = m_current;

        if (align > 1)
        {
            aligned_start = (aligned_start + (align-1)) & ~(align-1);
        }

        assert(aligned_start < m_end && "exceeds memory capacity\n");

        uintptr_t end = aligned_start + size;

        assert(m_start < end && "illegal memory address\n");
        assert(end <= m_end && "exceeds memory capacity\n");

        if (end > m_end)
           return nullptr;

        m_current = end;

        return reinterpret_cast<void*>(m_current);
    }

} // namespace Mif
