#pragma once

#include <cstdint>
#include <cmath>

constexpr float EPSILION = 2.2204460492503131e-16;

inline uint32_t RoundFloatUint32(float value)
{
    return static_cast<uint32_t>(value + 0.5f);
}

inline int32_t RoundFloatInt32(float value)
{
    return lroundf(value);
}

inline float FloorFloat(float value)
{
    return floorf(value);
}

inline int32_t FloorFloatInt32(float value)
{
    return static_cast<int32_t>(FloorFloat(value));
}

inline float CeilFloat(float value)
{
    return ceilf(value);
}

inline int32_t CeilFloatInt32(float value)
{
    return static_cast<int32_t>(CeilFloat(value));
}

inline bool EqualWithEpsilion(float a, float b)
{
    return fabsf(a - b) <= EPSILION;
}

inline float SquareRoot(float value)
{
    return sqrtf(value);
}

inline float Abs(float value)
{
    return fabs(value);
}

template<typename valType>
inline valType Square(valType val)
{
    return val * val;
}

template<typename valType>
inline valType Lerp(valType A, valType B, float t)
{
    valType result = (1.0f - t) * A + (B * t);
    return result;
}