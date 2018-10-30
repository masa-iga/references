#include <cstdio>
#include <cstdint>

// reference: http://www.swedishcoding.com/2008/08/31/are-we-out-of-memory/

namespace Mif {

    class StackAllocator {
    public:
        StackAllocator(void* start, size_t size);

        void* alloc(size_t size, size_t align);
        void freeToMarker(void* marker);

        uint32_t getSizeInBytes();
        uint32_t getRemainingSizeInBytes();

        inline void* getStart() { return m_start; }
        inline void* getMarker() { return m_current; }
        inline void* getEnd() { return m_end; }

    private:
        void* m_start;
        void* m_end;
        void* m_current;

    };

} // namespace Mif