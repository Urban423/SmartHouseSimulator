#pragma once
#include "Vector2.h"

class Vector2;

class Matrix3x3
{
public:
	Matrix3x3();
	//~Matrix4x4();

	void setIdentity();
	float getDeterminant();
	void inverse();
	
	void setScale(const Vector2& scale);
	void setTranslation(const Vector2& translation);
	void setRotation(const float rotation);

	float* getPtr();
public:
	void operator *=(const Matrix3x3& matrix);
	float* operator[](const unsigned int index);
private:
	float mat[3][3] = {0};
};