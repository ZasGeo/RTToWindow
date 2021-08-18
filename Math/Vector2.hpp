#pragma once

#include <cassert>
#include <Math/Math.hpp>

struct Vector2
{
    float x;
    float y;

    float& operator[](int index) { return (&x)[index]; }
};

inline Vector2 operator+(Vector2 lhs, Vector2 rhs)
{
    Vector2 result;
    result.x = lhs.x + rhs.x;
    result.y = lhs.y + rhs.y;
    return result;
}

inline Vector2 operator-(Vector2 lhs, Vector2 rhs)
{
    Vector2 result;
    result.x = lhs.x - rhs.x;
    result.y = lhs.y - rhs.y;
    return result;
}

inline Vector2 operator*(Vector2 lhs, Vector2 rhs)
{
    Vector2 result;
    result.x = lhs.x * rhs.x;
    result.y = lhs.y * rhs.y;
    return result;
}

inline Vector2 operator*(Vector2 vec, float scalar)
{
    Vector2 result;
    result.x = vec.x * scalar;
    result.y = vec.y * scalar;
    return result;
}

inline Vector2 operator*(float scalar, Vector2 vec)
{
    return vec * scalar;
}

inline Vector2 operator/(Vector2 vec, float scalar)
{
    float recip = 1.0f / scalar;
    Vector2 result = vec * recip;
    return result;
}

inline Vector2 operator-(Vector2 vec)
{
    Vector2 result;
    result.x = -vec.x;
    result.y = -vec.y;
    return result;
}

inline Vector2& operator+=(Vector2& operand, Vector2 value)
{
    operand = operand + value;
    return operand;
}

inline Vector2& operator-=(Vector2& operand, Vector2 value)
{
    operand = operand - value;
    return operand;
}

inline Vector2& operator*=(Vector2& operand, Vector2 value)
{
    operand = operand * value;
    return operand;
}

inline Vector2& operator*=(Vector2& operand, float value)
{
    operand = operand * value;
    return operand;
}

inline Vector2& operator/=(Vector2& operand, float value)
{
    operand = operand / value;
    return operand;
}

inline float Dot(Vector2 a, Vector2 b)
{
    float result = a.x * b.x + a.y * b.y;
    return result;
}

inline float LengthSq(Vector2 vec)
{
    return Dot(vec, vec);
}

inline float Length(Vector2 vec)
{
    float result = SquareRoot(LengthSq(vec));
    return result;
}

inline Vector2 Reflect(Vector2 vec, Vector2 normal)
{
    assert(EqualWithEpsilion(LengthSq(normal), 1.0f));
    Vector2 result = vec - 2.0f * Dot(vec, normal) * normal;
    return result;
}

inline Vector2 GetNormilized(Vector2 vec)
{
    float len = Length(vec);
    Vector2 result = EqualWithEpsilion(len, 0.0f) ? vec : vec / len;
    return result;
}