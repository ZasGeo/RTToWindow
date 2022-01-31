#pragma once

#include <cstdint>

#ifdef COMPILER_MSVC
#include <intrin.h>
#pragma intrinsic(_BitScanForward)
#endif

template<typename Type>
inline Type GetMax(Type a, Type b)
{
    return a >= b ? a : b;
}

template<typename Type>
inline Type GetMin(Type a, Type b)
{
    return a <= b ? a : b;
}

template<typename Type>
inline Type Clamp(Type value, Type min, Type max)
{
    return GetMin(GetMax(value, min), max);
}

struct BitTestRes
{
    uint32_t Index;
    bool IsSucceeded;
};

inline BitTestRes FindLeastSignificantBit(uint32_t value)
{
    BitTestRes result = {};
#ifdef COMPILER_MSVC
    result.IsSucceeded = _BitScanForward((unsigned long*)&result.Index, value);
#else
    for (uint32_t i = 0; i < 32; ++i)
    {
        if (value & (1 << i))
        {
            result.Index = i;
            result.IsSucceeded = true;
            break;
        }
    }
#endif
    return result;
}

struct BitScanResult
{
    bool Found;
    uint32_t Index;
};

inline BitScanResult FindLeastSignificantSetBit(uint32_t value)
{
    BitScanResult result = {};

#if COMPILER_MSVC
    result.Found = _BitScanForward((unsigned long*)&result.Index, value);
#else
    for (uint32_t test = 0;
        test < 32;
        ++test)
    {
        if (value & (1 << test))
        {
            result.Index = test;
            result.Found = true;
            break;
        }
    }
#endif

    return result;
}