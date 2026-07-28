#pragma once
#include "Vector2.h"
#include "stdlib.h"
#include "umath.h"

class Vector3
{
public:
	inline Vector3() : x(0), y(0), z(0) {};
	inline Vector3(float value) : x(value), y(value), z(value) {};
	inline Vector3(const Vector3 &vec3) : x(vec3.x), y(vec3.y), z(vec3.z) {};
	inline Vector3(const Vector2 &vec2) : x(vec2.x), y(vec2.y), z(0) {};
	inline Vector3(const Vector2 vec2, float z) : x(vec2.x), y(vec2.y), z(z) {};
	inline Vector3(const Vector2 vec2, char z) : x(vec2.x), y(vec2.y), z(z) {};
	inline Vector3(float x, float y, float z) : x(x), y(y), z(z) {};

	float length();
	inline float sqrMagnitude() { return x * x + y * y + z * z; }

	inline Vector3 normalized()
	{
		float l = sqrt(x * x + y * y + z * z);
		if (l == 0)
		{
			return Vector3(0, 0, 0);
		}
		return Vector3(x / l, y / l, z / l);
	}

public:
	static float Dot(Vector3 a, Vector3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
	static float Distance(Vector3 a, Vector3 b);
	static inline float SqrDistance(Vector3 a, const Vector3 &b)
	{
		a -= b;
		return a.x * a.x + a.y * a.y + a.z * a.z;
	}
	inline static Vector3 Cross(Vector3 a, Vector3 b) {
		return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
	}

public:
	Vector3 operator*(const float b);
	inline Vector3 operator/(const float b) { return {x / b, y / b, z / b}; }
	inline Vector3 operator*(const Vector3 b) { return {x * b.x, y * b.y, z * b.z}; };

	inline Vector3 &operator*=(const float b)
	{
		x *= b;
		y *= b;
		z *= b;
		return *this;
	}
	inline Vector3 &operator/=(const float b)
	{
		x /= b;
		y /= b;
		z /= b;
		return *this;
	}
	inline Vector3 &operator=(const Vector2 b)
	{
		x = b.x;
		y = b.y;
		z = 0;
		return *this;
	}
	inline float &operator[](const int index)
	{
		switch (index)
		{
		case (0):
		{
			return x;
		}
		case (1):
		{
			return y;
		}
		}
		return z;
	}
	inline const float &operator[](const int index) const {
		switch (index) {
		case (0): {
			return x;
		}
		case (1): {
			return y;
		}
		}
		return z;
	}
	inline bool operator==(const Vector3 vec3) { return x == vec3.x && y == vec3.y && z == vec3.z; }
	inline bool operator!=(const Vector3 vec3) { return x != vec3.x || y != vec3.y || z != vec3.z; }
	inline void operator+=(const Vector3 vec3) { this->x += vec3.x; this->y += vec3.y; this->z += vec3.z; }
	inline Vector3 operator/(float scalar) const { return {x / scalar, y / scalar, z / scalar}; }

	inline void operator-=(const Vector3 vec3)
	{
		this->x -= vec3.x;
		this->y -= vec3.y;
		this->z -= vec3.z;
	}

public:
	float x = 0;
	float y = 0;
	float z = 0;
};

inline Vector3 operator*(float a, const Vector3 &b) { return Vector3(b.x * a, b.y * a, b.z * a); }
inline Vector3 operator-(Vector3 b) { return Vector3(-b.x, -b.y, -b.z); }
inline Vector3 operator+(const Vector3 a, const Vector3 b) { return { a.x + b.x, a.y + b.y, a.z + b.z }; }
inline Vector3 operator-(const Vector3 a, const Vector3 b) { return { a.x - b.x, a.y - b.y, a.z - b.z }; }



inline void closestPointsSegmentSegment(const Vector3 p1, const Vector3 q1, const Vector3 p2, const Vector3 q2, Vector3& c1, Vector3& c2) {
	Vector3 d1 = q1 - p1;
    Vector3 d2 = q2 - p2;
    Vector3 r  = p1 - p2;

    float a = Vector3::Dot(d1,d1);
    float e = Vector3::Dot(d2,d2);
    float b = Vector3::Dot(d1,d2);
    float c = Vector3::Dot(d1,r);
    float f = Vector3::Dot(d2,r);

    float s = 0.0f;
    float t = 0.0f;

    const float EPS = 1e-6f;

    if (a <= EPS && e <= EPS) {
        c1 = p1;
        c2 = p2;
        return;
    }

    if (a <= EPS) {
        t = Math::clamp(f/e,0.0f,1.0f);
    }
    else {
        float denom = a*e - b*b;
        if (fabs(denom) > EPS) s = Math::clamp((b*f-c*e)/denom,0.0f,1.0f);
        t = (b*s+f)/e;
        if(t < 0) {
            t = 0;
            s = Math::clamp(-c/a,0.0f,1.0f);
        }
        else if(t > 1) {
            t = 1;
            s = Math::clamp((b-c)/a,0.0f,1.0f);
        }
    }
    c1 = p1 + d1*s;
    c2 = p2 + d2*t;
}

inline Vector3 closestPointOnSegment(const Vector3 p, const Vector3 a, const Vector3 b) {
    Vector3 ab = b - a;
    float t = Vector3::Dot(p - a, ab) / Vector3::Dot(ab, ab);
    t = Math::clamp(t, 0.0f, 1.0f);
    return a + ab * t;
}