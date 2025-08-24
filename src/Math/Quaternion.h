#pragma once
#include "Vector3.h"

class Quaternion
{
public:
	Quaternion();
	Quaternion(float x, float y, float z, float w);
	
	void normalize();
	static Quaternion Inverse(const Quaternion q);
public:
	Quaternion operator+(const Quaternion q);
	Quaternion operator-(const Quaternion q);
	Quaternion operator*(const Quaternion b);
	void operator*=(const Quaternion b);
	Quaternion operator*(const float b);
	Vector3 operator*(const Vector3 v);
public:
	float x = 0;
	float y = 0;
	float z = 0;
	float w = 1;
};