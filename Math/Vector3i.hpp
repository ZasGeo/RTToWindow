#pragma once

#include <Math/Vector3.hpp>

struct Vector3i
{
	int32_t x;
	int32_t y;
	int32_t z;
};

inline bool operator==(Vector3i left, Vector3i right)
{
	bool result = left.x == right.x && left.y == right.y && left.z == right.z;
	return result;
}

inline bool operator!=(Vector3i left, Vector3i right)
{
	return !(left == right);
}

inline Vector3i operator+(Vector3i left, Vector3i right)
{
	Vector3i result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;
	return result;
}

inline Vector3i operator-(Vector3i left, Vector3i right)
{
	Vector3i result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	return result;
}

inline Vector3 operator*(Vector3i left, Vector3 right)
{
	Vector3 result;
	result.x = left.x * right.x;
	result.y = left.y * right.y;
	result.z = left.z * right.z;
	return result;
}

inline Vector3i Vector3iFromVector3(Vector3 from)
{
	Vector3i result;
	result.x = RoundFloatInt32(from.x);
	result.y = RoundFloatInt32(from.y);
	result.z = RoundFloatInt32(from.z);
	return result;
}

inline Vector3 Vector3FromVector3i(Vector3i from)
{
	Vector3 result;
	result.x = (float)from.x;
	result.y = (float)from.y;
	result.z = (float)from.z;
	return result;
}