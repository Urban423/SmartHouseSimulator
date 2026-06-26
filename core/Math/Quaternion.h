#pragma once
#include "Vector3.h"

class Quaternion
{
public:
	Quaternion() : x(0), y(0), z(0), w(1) {};
	Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

	void normalize();
	static Quaternion Inverse(const Quaternion q);
	static Quaternion FromEuler(float pitch, float yaw, float roll);
	static Quaternion FromAxisAngle(const Vector3& axis, float angle);

public:
	Quaternion operator+(const Quaternion q);
	Quaternion operator-(const Quaternion q);
	inline Quaternion  operator*(const Quaternion& b);
	inline Quaternion& operator*=(const Quaternion& b);
	Quaternion operator*(const float b);
	Vector3 operator*(const Vector3 v);

public:
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;
};




inline Quaternion Quaternion::FromAxisAngle(const Vector3& axis, float angle)
{
    float halfAngle = angle * 0.5f;

    float s = sin(halfAngle);
    float c = cos(halfAngle);

    Quaternion q;
    q.x = axis.x * s;
    q.y = axis.y * s;
    q.z = axis.z * s;
    q.w = c;

    return q;
}

inline Quaternion Quaternion::FromEuler(float pitch, float yaw, float roll)
{
    float d2r = 3.14159265f / 180.0f;

    pitch *= d2r;
    yaw   *= d2r;
    roll  *= d2r;

    Quaternion qx = Quaternion::FromAxisAngle(Vector3(1,0,0), pitch);
    Quaternion qy = Quaternion::FromAxisAngle(Vector3(0,1,0), yaw);
    Quaternion qz = Quaternion::FromAxisAngle(Vector3(0,0,1), roll);

    // FPS-style order (VERY IMPORTANT)
    return qy * qx * qz;
}

inline Quaternion Quaternion::operator*(const Quaternion& b)
{
	return Quaternion(
		 x * b.w + y * b.z - z * b.y + w * b.x,
		-x * b.z + y * b.w + z * b.x + w * b.y,
		 x * b.y - y * b.x + z * b.w + w * b.z,
		-x * b.x - y * b.y - z * b.z + w * b.w);
}

inline Quaternion& Quaternion::operator*=(const Quaternion& b)
{
	float newX =   x * b.w + y * b.z - z * b.y + w * b.x;
	float newY =  -x * b.z + y * b.w + z * b.x + w * b.y;
	float newZ =   x * b.y - y * b.x + z * b.w + w * b.z;
	float newW =  -x * b.x - y * b.y - z * b.z + w * b.w;
	
	this->x = newX;
	this->y = newY;
	this->z = newZ;
	this->w = newW;
	return *this;
}