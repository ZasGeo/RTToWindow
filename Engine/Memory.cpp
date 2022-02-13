#include <Engine/Memory.h>
#include <cassert>

void MemoryBlock::Initialize(uint8_t* base, uint64_t size)
{
    m_Base = base;
    m_TotalSize = size;
    m_Used = 0;
}

void* MemoryBlock::PushMemory(uint64_t size)
{
    assert(size >= 4);
    assert(m_Used + size <= m_TotalSize);

    void* result = m_Base + m_Used;
    m_Used += size;

    return result;
}
