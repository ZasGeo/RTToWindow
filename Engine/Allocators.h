#pragma once

#include <cstdint>

class LinearAllocator
{
public:
    void Initialize(uint8_t* base, uint64_t size);
    void* PushMemory(uint64_t size);
    void Clear();

private:
    uint8_t* m_Base{ nullptr };
    uint64_t m_Used{ 0 };
    uint64_t m_TotalSize{ 0 };
};

#define LinearAllocate(Allocator, Type) (Type*)(Allocator.PushMemory(sizeof(Type)))
#define LinearAllocateArray(Allocator, Count, Type) (Type*)(Allocator.PushMemory((Count) * sizeof(Type)))