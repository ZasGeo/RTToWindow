#pragma once

#include <cstdint>

class MemoryBlock
{
public:
    void Initialize(uint8_t* base, uint64_t size);
    void* PushMemory(uint64_t size);

private:
    uint8_t* m_Base{ nullptr };
    uint64_t m_Used{ 0 };
    uint64_t m_TotalSize{ 0 };
};

#define PushStruct(MemoryBlock, Type) (Type*)((MemoryBlock).PushMemory(sizeof(Type)))
#define PushSize(MemoryBlock, Size) ((MemoryBlock).PushMemory((Size)))