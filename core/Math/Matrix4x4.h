#pragma once
#include "Vector3.h"
#include "Quaternion.h"

class Matrix4x4
{
public:
	Matrix4x4() {}
	Matrix4x4(const Matrix4x4 &) = default;
	Matrix4x4(Matrix4x4 &&) noexcept = default;
	//~Matrix4x4() {}

	void setIdentity();
	float getDeterminant();
	void inverse();

	void setScale(const Vector3 &scale);
	void setTranslation(const Vector3 &translation);
	void addTranslation(const Vector3 &translation);
	void setRotationX(const float x);
	void setRotationY(const float y);
	void setRotationZ(const float z);
	void setRotation(const Quaternion rotation);

	void setOrthoLH(float left, float right, float bottom, float top, float nearPlane, float farPlane);
	void setPerspectiveFovLH(float fov, float aspect, float nearPlane, float farPlane);

	Vector3 getXDirection();
	Vector3 getYDirection();
	Vector3 getZDirection();
	Quaternion getRotation() const;
	inline Vector3 getTranslation() { return {mat[3][0], mat[3][1], mat[3][2]}; }

	inline float *getPtr() { return &mat[0][0]; };
public:
	void operator*=(const Matrix4x4 &matrix);
	Matrix4x4 operator*(const Matrix4x4 matrix);
	inline float &operator[](const int index) { return *(&mat[0][0] + index); }
	Matrix4x4 &operator=(const Matrix4x4 &) = default;
	Matrix4x4 &operator=(Matrix4x4 &&) noexcept = default;

private:
	float mat[4][4] = {0};
};