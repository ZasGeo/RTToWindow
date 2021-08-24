#pragma once

#include <cassert>
#include <Math/Math.hpp>

struct Vector3
{
    float x;
    float y;
    float z;

    float& operator[](int index) { return (&x)[index]; }
};

inline Vector3 operator+(Vector3 lhs, Vector3 rhs)
{
    Vector3 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    result.z = lhs.z + rhs.z;
    return result;
}

inline Vector3 operator-(Vector3 lhs, Vector3 rhs)
{
    Vector3 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    result.z = lhs.z - rhs.z;
    return result;
}

inline Vector3 operator*(Vector3 lhs, Vector3 rhs)
{
    Vector3 result;
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    result.z = lhs.z * rhs.z;
    return result;
}

inline Vector3 operator*(Vector3 vec, float scalar)
{
    Vector3 result;
    result.x = vec.x * scalar;
    result.y = vec.y * scalar;
    result.z = vec.z * scalar;
    return result;
}

inline Vector3 operator*(float scalar, Vector3 vec)
{
    return vec * scalar;
}

inline Vector3 operator/(Vector3 vec, float scalar)
{
    float recip = 1.0f / scalar;
    Vector3 result = vec * recip;
    return result;
}

inline Vector3 operator-(Vector3 vec)
{
    Vector3 result;
    result.x = -vec.x;
    result.y = -vec.y;
    result.z = -vec.z;
    return result;
}

inline Vector3& operator+=(Vector3& operand, Vector3 value)
{
    operand = operand + value;
    return operand;
}

inline Vector3& operator-=(Vector3& operand, Vector3 value)
{
    operand = operand - value;
    return operand;
}

inline Vector3& operator*=(Vector3& operand, Vector3 value)
{
    operand = operand * value;
    return operand;
}

inline Vector3& operator*=(Vector3& operand, float value)
{
    operand = operand * value;
    return operand;
}

inline Vector3& operator/=(Vector3& operand, float value)
{
    operand = operand / value;
    return operand;
}

inline float Dot(Vector3 a, Vector3 b)
{
    float result = a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

inline float LengthSq(Vector3 vec)
{
    return Dot(vec, vec);
}

inline float Length(Vector3 vec)
{
    float result = SquareRoot(LengthSq(vec));
    return result;
}

inline Vector3 Reflect(Vector3 vec, Vector3 normal)
{
    assert(EqualWithEpsilion(LengthSq(normal), 1.0f));
    Vector3 result = vec - 2.0f * Dot(vec, normal) * normal;
    return result;
}

inline Vector3 GetNormilized(Vector3 vec)
{
    float len = Length(vec);
    Vector3 result = EqualWithEpsilion(len, 0.0f) ? vec : vec / len;
    return result;
}