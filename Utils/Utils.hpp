#pragma once

#include <cstdint>

namespace Utils
{
    uint32_t RoundFloatUint32(float value)
    {
        return static_cast<uint32_t>(value + 0.5f);
    }

    int32_t RoundFloatInt32(float value)
    {
        return static_cast<int32_t>(value + 0.5f);
    }

    template<typename Type>
    Type GetMax(Type a, Type b)
    {
        return a >= b ? a : b;
    }

    template<typename Type>
    Type GetMin(Type a, Type b)
    {
        return a <= b ? a : b;
    }

    template<typename Type>
    Type Clamp(Type value, Type min, Type max)
    {
        return GetMin(GetMax(value, min), max);
    }
}
