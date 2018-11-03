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


    void* StackAllocator::alloc(size_t size, size_t alignment /* must be 2^x */)
    {
        assert((alignment & (alignment-1)) == 0 && "alignment must be exponential of 2");

        const uintptr_t raw_address = m_current + size + alignment;
        const size_t mask = alignment - 1;
        const uintptr_t misalignment = raw_address & mask;
        const uintptr_t adjustment = alignment - misalignment;

        m_current = raw_address - adjustment;
        assert(m_current <= m_end && "exceeed memory capacity\n");

        return reinterpret_cast<void*>(m_current);
    }

} // namespace Mif
