#pragma once

#include <cassert>
#include <Math/Math.hpp>

struct Vector4
{
    float x;
    float y;
    float z;
    float w;

    float& operator[](int index) { return (&x)[index]; }
};

inline Vector4 operator+(Vector4 lhs, Vector4 rhs)
{
    Vector4 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    result.w = lhs.w + rhs.w;
    return result;
}

inline Vector4 operator-(Vector4 lhs, Vector4 rhs)
{
    Vector4 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    result.w = lhs.w - rhs.w;
    return result;
}

inline Vector4 operator*(Vector4 lhs, Vector4 rhs)
{
    Vector4 result;
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    result.z = lhs.z * rhs.z;
    result.w = lhs.w * rhs.w;
    return result;
}

inline Vector4 operator*(Vector4 vec, float scalar)
{
    Vector4 result;
    result.x = vec.x * scalar;
    result.y = vec.y * scalar;
    result.z = vec.z * scalar;
    result.w = vec.w * scalar;
    return result;
}

inline Vector4 operator*(float scalar, Vector4 vec)
{
    return vec * scalar;
}

inline Vector4 operator/(Vector4 vec, float scalar)
{
    float recip = 1.0f / scalar;
    Vector4 result = vec * recip;
    return result;
}

inline Vector4 operator-(Vector4 vec)
{
    Vector4 result;
    result.x = -vec.x;
    result.y = -vec.y;
    result.z = -vec.z;
    result.w = -vec.w;
    return result;
}

inline Vector4& operator+=(Vector4& operand, Vector4 value)
{
    operand = operand + value;
    return operand;
}

inline Vector4& operator-=(Vector4& operand, Vector4 value)
{
    operand = operand - value;
    return operand;
}

inline Vector4& operator*=(Vector4& operand, Vector4 value)
{
    operand = operand * value;
    return operand;
}

inline Vector4& operator*=(Vector4& operand, float value)
{
    operand = operand * value;
    return operand;
}

inline Vector4& operator/=(Vector4& operand, float value)
{
    operand = operand / value;
    return operand;
}

inline float Dot(Vector4 a, Vector4 b)
{
    float result = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    return result;
}

inline float LengthSq(Vector4 vec)
{
    return Dot(vec, vec);
}

inline float Length(Vector4 vec)
{
    float result = SquareRoot(LengthSq(vec));
    return result;
}

inline Vector4 Reflect(Vector4 vec, Vector4 normal)
{
    assert(EqualWithEpsilion(LengthSq(normal), 1.0f));
    Vector4 result = vec - 2.0f * Dot(vec, normal) * normal;
    return result;
}

inline Vector4 GetNormilized(Vector4 vec)
{
    float len = Length(vec);
    Vector4 result = EqualWithEpsilion(len, 0.0f) ? vec : vec / len;
    return result;
}