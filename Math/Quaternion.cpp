#include "Quaternion.h"
#include <math.h>

Quaternion::Quaternion(){}

Quaternion::Quaternion(float x, float y, float z, float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

Quaternion Quaternion::Inverse(const Quaternion q)
{
	return Quaternion(-q.x, -q.y, -q.z, q.w);
}

void Quaternion::normalize()
{
	float length = sqrt(w * w + x * x + y * y + z * z);
	if (length == 0) {
		return;
	}
	x /= length;
	y /= length;
	z /= length;
	w /= length;
}


Quaternion Quaternion::operator+(const Quaternion q)
{
	return Quaternion(this->x + q.x, this->y + q.y, this->z + q.z, this->w + q.w);
}

Quaternion Quaternion::operator-(const Quaternion q)
{
	return Quaternion(this->x - q.x, this->y - q.y, this->z - q.z, this->w - q.w);
}

Quaternion Quaternion::operator*(const Quaternion b)
{
	return Quaternion(
		 x * b.w + y * b.z - z * b.y + w * b.x,
		-x * b.z + y * b.w + z * b.x + w * b.y,
		 x * b.y - y * b.x + z * b.w + w * b.z,
		-x * b.x - y * b.y - z * b.z + w * b.w);
}

void Quaternion::operator*=(const Quaternion b)
{
	float newX =   x * b.w + y * b.z - z * b.y + w * b.x;
	float newY =  -x * b.z + y * b.w + z * b.x + w * b.y;
	float newZ =   x * b.y - y * b.x + z * b.w + w * b.z;
	float newW =  -x * b.x - y * b.y - z * b.z + w * b.w;
	
	this->x = newX;
	this->y = newY;
	this->z = newZ;
	this->w = newW;
}


Quaternion Quaternion::operator*(const float b)
{
	return Quaternion(this->x * b, this->y * b, this->z * b, this->w * b);
}



Vector3 Quaternion::operator*(const Vector3 v)
{
	Quaternion q = *this * Quaternion(v.x, v.y, v.z, 0) * Inverse(*this);
	return Vector3(q.x, q.y, q.z);
}