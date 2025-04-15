#pragma once
#include "Matrix3x3.h"
#include <math.h>

class Matrix3x3;

class Vector2
{
public:
	Vector2(): x(0), y(0) {};
	Vector2(float x, 	float y): x(x), y(y) {};
	Vector2(double x, 	double y): x(x), y(y) {};
	Vector2(int x, 		int y): x(x), y(y) {};
	Vector2(long x, 	long y): x(x), y(y) {};

	float length();
	float squareLength();
	Vector2 normalized();
public:
	Vector2 operator+(const Vector2& b);
	Vector2 operator-();
	Vector2 operator-(const Vector2& b);
	Vector2 operator*(float b);
	Vector2 operator*(Matrix3x3& matrix);
	Vector2 operator*=(Matrix3x3& matrix);
	bool operator<(const Vector2& b) const;
	Vector2 operator/(float b);
	inline Vector2 operator/(const Vector2 b) { return {x / b.x, y / b.y}; }
	Vector2& operator/=(float b);
	Vector2 operator+=(const Vector2& b);
	Vector2 operator-=(const Vector2& b);
	Vector2 operator*=(float b);
	float& operator[](unsigned int index);
	const float& operator[](unsigned int index) const;
	bool operator==(const Vector2& b);
public:
	static float Distance(const Vector2& a, const Vector2& b);
	static float DistanceSquare(const Vector2& a, const Vector2& b);
	static float angle(const Vector2& v) { return std::atan2(v.y, v.x); }
	static float dotProduct(const Vector2& a, const Vector2& b) {return a.x * b.x + a.y * b.y;}
	static Vector2 minDistToLine(Vector2& a, Vector2& b, Vector2& point, float (*distFunc)(const Vector2&, const Vector2&));
	static char  linesItersection(Vector2& intersectPointA, Vector2& intersectPointB, const Vector2& a1, const Vector2& b1, const Vector2& a2, const Vector2& b2);
public:
	float x = 0;
	float y = 0;
};

inline bool clockwise       (const Vector2& a, const Vector2& b, const Vector2& c) { return ((b.y - a.y) * (c.x - b.x) - (c.y - b.y) * (b.x - a.x)) >= 0;}
inline bool counterClockwise(const Vector2& a, const Vector2& b, const Vector2& c) { return ((b.y - a.y) * (c.x - b.x) - (c.y - b.y) * (b.x - a.x)) < 0;}
Vector2 operator*(float a, const Vector2& b);
