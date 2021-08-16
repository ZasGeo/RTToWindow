#pragma once

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

inline float Dot2(Vector2 a, Vector2 b)
{
    float result = a.x * b.x + a.y * b.y;
    return result;
}

inline Vector2 Reflect2(Vector2 vec, Vector2 normal)
{
    //#TODO assert on normal len = 1
    Vector2 result = vec - 2.0f * Dot2(vec, normal) * normal;
    return result;
}