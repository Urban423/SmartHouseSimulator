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

Vector3 Vector3::operator*(const float b)
{
	return Vector3(this->x * b, this->y * b, this->z * b);
}