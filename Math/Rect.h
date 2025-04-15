#pragma once
#include "Matrix3x3.h"
#include "Vector3.h"

class Rect
{
public:
	Rect() {};
	Rect(float width, float height): right(width), bottom(height) { }
	Rect(float left, float top, float right, float bottom): right(right), bottom(bottom), left(left), top(top) { }
	Rect(const Rect& rect): right(rect.right), bottom(rect.bottom), left(rect.left), top(rect.top) { }
	//~Rect();
	
	inline bool contains(float x, float y) const { return (x >= left && x <= right && y >= top && y <= bottom); }
	
	inline float width()  { return right - left;}
	inline float height() { return bottom - top;}
public:
	Rect& operator*=(const float b) { left *= b; top *= b; right *= b; bottom *= b; return *this;}
	Rect operator*(const Vector3 b) { return {left * b.x, top * b.y, right * b.x, bottom * b.y}; }
	Rect operator+(const Vector3 b) { return {left + b.x, top + b.y, right + b.x, bottom + b.y}; }
	Vector2 operator[](const int index) { 
		switch(index) {
			case(0): { return {left, 	top}; };
			case(1): { return {left, 	bottom}; };
			case(2): { return {right, 	top}; };
			case(3): { return {right, 	bottom}; };
		}
		return {left, top};
	}
public:
	float left  	= 0;
	float right  	= 0;
	float bottom 	= 0;
	float top 		= 0;
};

//returns an array with number_of_rects rects in random position and scale all of the rects are scaled inside rect {0, 0, 1, 1}
//std::vector<Rect> generateBSP(std::vector<pofloats>& begin, end, )