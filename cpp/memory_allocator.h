#include <cstdio>
#include <cstdint>

// reference: http://www.swedishcoding.com/2008/08/31/are-we-out-of-memory/

namespace Mif {

    class StackAllocator {
    public:
        StackAllocator(void* start, size_t size);

        void* alloc(size_t size, size_t align);
        //void freeToMarker(void* marker);
        void reset() { m_current = m_start; };

        void* getStart() { return reinterpret_cast<void*>(m_start); }
        void* getCurrent() { return reinterpret_cast<void*>(m_current); }
        void* getEnd() { return reinterpret_cast<void*>(m_end); }
        uint32_t getSizeInBytes() { return static_cast<uint32_t>(m_current - m_start); }
        uint32_t getRemainingSizeInBytes() { return static_cast<uint32_t>(m_end - m_current); }

    private:
        uintptr_t m_start;
        uintptr_t m_end;
        uintptr_t m_current;

    };

} // namespace Mif
