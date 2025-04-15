#include "Vector3.h"

float Vector3::length()
{
	return sqrt(x * x + y * y + z * z);
}


float Vector3::Distance(Vector3 a, Vector3 b)
{
	a -= b;
	return sqrt(a.x * a.x + a.y * a.y + a.z * a.z); 
}

Vector3 Vector3::Cross(Vector3 a, Vector3 b)
{
	return Vector3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

Vector3 Vector3::operator+(const Vector3 vec3)
{
	return Vector3(this->x + vec3.x, this->y + vec3.y, this->z + vec3.z);
}

Vector3 Vector3::operator-(const Vector3& vec3)
{
	return Vector3(this->x - vec3.x, this->y - vec3.y, this->z - vec3.z);
}

Vector3 Vector3::operator*(const float b)
{
	return Vector3(this->x * b, this->y * b, this->z * b);
}