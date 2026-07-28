#pragma once
#include "Vector2.h"

#define RGB_TO_INT(r, g, b) (((r) << 16) + ((g) << 8) + (b))

struct TextureStruct
{
	int width;
	int height;
	int* pixels;
};

namespace Math {
    constexpr float PI = 3.14159265358979323846f;
	
	inline float Clamp(float a, float b, float value) {
		if(value < a) return a;
		if(value > b) return b;
		return value;
	}
	template<class T>
	inline T Max(T a, T b) {
		if(a < b) return b;
		return a;
	}
	template<class T>
	inline T Min(T a, T b) {
		if(a > b) return b;
		return a;
	}

	inline float clamp(float value, float a, float b) {
		if(value < a) return a;
		if(value > b) return b;
		return value;
	}
};

inline float EuclideanDistance(const Vector2& a, const Vector2& b) { return (b - a).length(); }
inline float EuclideanDistanceSq(const Vector2& a, const Vector2& b) { return (b - a).squareLength(); }
inline float ManhattanDistance(const Vector2& a, const Vector2& b) { return fabs(b.x - a.x) + fabs(b.y - a.y);}
inline float ChebyshevDistance(const Vector2& a, const Vector2& b) { return Math::Max(fabs(b.x - a.x), fabs(b.y - a.y)); }

inline float clamp(float a, float b, float value) {
	if(value < a) return a;
	if(value > b) return b;
	return value;
}


inline float Q_rsqrt( float number )
{	
	const float x2 = number * 0.5F;
	const float threehalfs = 1.5F;

	union {
		float f;
		unsigned int i;
	} conv = {number}; // member 'f' set to value of 'number'.
	conv.i = 0x5f3759df - ( conv.i >> 1 );
	conv.f *= threehalfs - x2 * conv.f * conv.f;
	return conv.f;
}