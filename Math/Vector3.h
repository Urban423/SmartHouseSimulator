#include "Vector2.h"
#include <math.h>

#ifndef VECTOR3_H
#define VECTOR3_H

class Vector3
{
public:
	inline Vector3(): x(0), y(0), z(0) {};
	inline Vector3(const Vector3& vec3): x(vec3.x), y(vec3.y), z(vec3.z) {};
	inline Vector3(float x, float y, float z): x(x), y(y), z(z) {};
	
	float length();
	
	inline float sqrMagnitude()
	{
		return x * x + y * y + z * z;
	}
	
	inline Vector3 normalized()
	{
		float l = sqrt(x * x + y * y + z * z);
		if(l == 0) {
			return Vector3(0, 0, 0);
		}
		return Vector3(x / l, y / l, z / l);
	}
public:
	static float Distance(Vector3 a, Vector3 b);
	static inline float SqrDistance(Vector3 a, const Vector3& b)
	{
		a -= b;
		return a.x * a.x + a.y * a.y + a.z * a.z; 
	}
	static Vector3 Cross(Vector3 a, Vector3 b);
public:
	Vector3 operator*(const float b);
	inline Vector3 operator/(const float b) 	{ return {x / b, y / b, z / b}; }
	inline Vector3 operator*(const Vector3 b) 	{ return {x * b.x, y * b.y, z * b.z}; };
	
	inline Vector3& operator*=(const float b) { x *= b; y *= b; z *= b; return *this; }
	inline Vector3& operator/=(const float b) { x /= b; y /= b; z /= b; return *this; }
	inline Vector3& operator=(const Vector2 b) { x = b.x; y = b.y; z = 0; return *this; }
	inline float&   operator[](const int index) {
		switch(index){
			case(0): {return x; }
			case(1): {return y; }
		}
		return z;
	}
	inline const float& operator[](const int index) const {
		switch(index){
			case(0): {return x; }
			case(1): {return y; }
		}
		return z;
	}
	
	Vector3 operator+(const Vector3 vec3);
	Vector3 operator-(const Vector3& vec3);
	
	inline void operator+=(const Vector3 vec3)
	{
		this->x += vec3.x;
		this->y += vec3.y;
		this->z += vec3.z;
	}
	
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

inline Vector3 operator*(float a, const Vector3& b) { return Vector3(b.x * a, b.y * a, b.z * a);}

#endif // VECTOR3_H