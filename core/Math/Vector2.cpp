#include "Vector2.h"
#include <math.h>

float Vector2::Distance(const Vector2& a, const Vector2& b)
{
	float _x = b.x - a.x;
	float _y = b.y - a.y;
	
	return sqrt(_x * _x + _y * _y);
}
	
float Vector2::DistanceSquare(const Vector2& a, const Vector2& b)
{
	float _x = b.x - a.x;
	float _y = b.y - a.y;
	
	return _x * _x + _y * _y;
}

char Vector2::linesItersection(Vector2& intersectPointA, Vector2& intersectPointB, const Vector2& a1, const Vector2& b1, const Vector2& a2, const Vector2& b2) {
	if((a1.x == b1.x && a1.y == b1.y) || (a2.x == b2.x && a2.y == b2.y)) { return 0;}
	
	float denominator = (b2.y - a2.y) * (b1.x - a1.x) - (b2.x - a2.x) * (b1.y - a1.y);
	
	if(denominator == 0) {
		float crossProduct = (a2.y - a1.y) * (b1.x - a1.x) - (a2.x - a1.x) * (b1.y - a1.y);
		if(crossProduct != 0) { return 0; }
		
		
		Vector2 start = {std::max(std::min(a1.x, b1.x), std::min(a2.x, b2.x)), std::max(std::min(a1.y, b1.y), std::min(a2.y, b2.y))};
		Vector2 end   = {std::min(std::max(a1.x, b1.x), std::max(a2.x, b2.x)), std::min(std::max(a1.y, b1.y), std::max(a2.y, b2.y))};
		if (start.x > end.x || start.y > end.y) {
			return 0;
		}
		
		intersectPointA = start;
		intersectPointB = end;
		
		return 2;
	}
	
	float ua = ((b2.x - a2.x) * (a1.y - a2.y) - (b2.y - a2.y) * (a1.x - a2.x)) / denominator;
	float ub = ((b1.x - a1.x) * (a1.y - a2.y) - (b1.y - a1.y) * (a1.x - a2.x)) / denominator;
	
	if(ua < 0 || ua > 1 || ub < 0 || ub > 1) { return 0; }
	
	intersectPointA = {a1.x + ua * (b1.x - a1.x), a1.y + ua * (b1.y - a1.y)};
	return 1;
}

Vector2 Vector2::minDistToLine(Vector2& a, Vector2& b, Vector2& point, float (*distFunc)(const Vector2&, const Vector2&)) {
        const int SAMPLES = 100; // Increase for more accuracy
        Vector2 closest = a;
        float minDist = distFunc(point, a);

        for (int i = 1; i <= SAMPLES; i++) {
            float t = i / static_cast<float>(SAMPLES);
            Vector2 sample = a + (b - a) * t;
            float d = distFunc(point, sample);

            if (d < minDist) {
                minDist = d;
                closest = sample;
            }
        }

        return closest;
    }






float Vector2::length()
{
	return sqrt(x * x + y * y);
}

float Vector2::squareLength()
{
	return x * x + y * y;
}


Vector2 Vector2::normalized()
{
	float _length = 1.0f / sqrt(x * x + y * y);
	return Vector2(x * _length, y * _length);
}

Vector2 Vector2::operator+(const Vector2& b)
{
	return Vector2(x + b.x, y + b.y);
}

Vector2 Vector2::operator-()
{
	return Vector2(-x, -y);
}

Vector2 Vector2::operator-(const Vector2& b)
{
	return Vector2(x - b.x, y - b.y);
}

Vector2 Vector2::operator*(float b)
{
	return Vector2(x * b, y * b);
}

Vector2 Vector2::operator*(Matrix3x3& matrix) {
	return Vector2(matrix[0][0] * x + matrix[0][1] * y + matrix[2][0], matrix[1][0] * x + matrix[1][1] * y + matrix[2][1]);
}

Vector2 Vector2::operator*=(Matrix3x3& matrix) {
	*this = *this * matrix;
	return *this;
}

bool Vector2::operator<(const Vector2& b) const
{
	return x < b.x || (x == b.x && y < b.y);
}

Vector2 Vector2::operator/(float b)
{
	return Vector2(x / b, y / b);
}

Vector2& Vector2::operator/=(float b) {
	x /= b;
	y /= b;
	return *this;
}

Vector2 Vector2::operator+=(const Vector2& b)
{
	x += b.x;
	y += b.y;
	return *this;
}

Vector2 Vector2::operator-=(const Vector2& b)
{
	x -= b.x;
	y -= b.y;
	return *this;
}

Vector2 Vector2::operator*=(float b)
{
	x *= b;
	y *= b;
	return *this;
}

bool Vector2::operator==(const Vector2& b)
{
	return x == b.x && y == b.y;
}

float& Vector2::operator[](unsigned int index) { if(index) { return y; } return x; }

const float& Vector2::operator[](unsigned int index) const { if(index) { return y; } return x; }

Vector2 operator*(float a, const Vector2& b)
{
	return Vector2(a * b.x, a * b.y);
}
