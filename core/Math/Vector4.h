#pragma once
#include "Vector2.h"
#include "Vector3.h"

class Vector4
{
public:
	Vector4();
	Vector4(float value): x(value), y(value), z(value), w(value) {}
	Vector4(float x, float y, float z, float w);
	//~Vector4();

	static Vector4 cross(const Vector4& v1, const Vector4& v2, const Vector4& v3);
public:
	inline float& operator[](const int index) {
		switch(index) {
			case(0): { return x; }
			case(1): { return y; }
			case(2): { return z; }
		}
		return w;
	}
	inline Vector4& operator*=(float scale) {
		x *= scale;
		y *= scale;
		z *= scale;
		w *= scale;
		return *this;
	}
	inline Vector4 operator*(float scale) {
		return {x * scale, y * scale, z * scale, w * scale};
	}
public:
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;
};