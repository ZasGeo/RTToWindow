#pragma once

#include <cstdint>
#include <cmath>

constexpr float EPSILION = 2.2204460492503131e-16;

inline uint32_t RoundFloatUint32(float value)
{
    return static_cast<uint32_t>(value + 0.5f);
}

inline float FloorFloat(float value)
{
    return floorf(value);
}

inline int32_t FloorFloatInt32(float value)
{
    return static_cast<int32_t>(FloorFloat(value));
}

inline bool EqualWithEpsilion(float a, float b)
{
    return fabsf(a - b) <= EPSILION;
}

inline float SquareRoot(float value)
{
    return sqrtf(value);
}