#pragma once
#include "Matrix3x3.h"
#include "stdlibInclude.h"

class Matrix3x3;
class Vector3;

class Vector2
{
public:
	Vector2(): x(0), y(0) {};
	Vector2(float x, 	float y): x(x), y(y) {};
	Vector2(double x, 	double y): x(x), y(y) {};
	Vector2(int x, 		int y): x(x), y(y) {};
	Vector2(long x, 	long y): x(x), y(y) {};
	Vector2(const Vector3 b);

	Vector2& rotate(float radianAngle);
	float length();
	float squareLength();
	Vector2 normalized();
public:
	Vector2& operator=(const Vector3 b);
	Vector2 operator-();
	Vector2 operator*(float b);
	inline Vector2 operator*(Vector2 b) { return {x * b.x, y * b.y}; }
	Vector2 operator*(Matrix3x3& matrix);
	Vector2 operator*=(Matrix3x3& matrix);
	bool operator<(const Vector2& b) const;
	bool operator>(const Vector2& b) const;
	Vector2 operator/(float b);
	inline Vector2 operator/(const Vector2 b) { return {x / b.x, y / b.y}; }
	Vector2& operator/=(float b);
	Vector2 operator+=(const Vector2& b);
	inline Vector2& operator+=(const int b) { x += b; y += b; return *this; }
	Vector2 operator-=(const Vector2& b);
	Vector2 operator*=(float b);
	float& operator[](unsigned int index);
	const float& operator[](unsigned int index) const;
	bool operator==(const Vector2& b);
public:
	static float DistanceToSegment(const Vector2& a, const Vector2& b, const Vector2& p,
                        float (*distFunc)(const Vector2&, const Vector2&) = Vector2::Distance);
	static float Distance(const Vector2& a, const Vector2& b);
	static float DistanceSquare(const Vector2& a, const Vector2& b);
	static float angle(const Vector2& v) { return std::atan2(v.y, v.x); }
	static float Dot(const Vector2& a, const Vector2& b) {return a.x * b.x + a.y * b.y;}
	static Vector2 minDistToLine(Vector2& a, Vector2& b, Vector2& point, float (*distFunc)(const Vector2&, const Vector2&));
	static char  linesItersection(Vector2& intersectPointA, Vector2& intersectPointB, const Vector2& a1, const Vector2& b1, const Vector2& a2, const Vector2& b2);
public:
	float x = 0;
	float y = 0;
};

inline bool clockwise(const Vector2& a, const Vector2& b, const Vector2& c) {
    return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x) < 0;
}

inline bool counterClockwise(const Vector2& a, const Vector2& b, const Vector2& c) {
    return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x) > 0;
}
Vector2 operator*(float a, const Vector2& b);
inline Vector2 operator-(const Vector2 a, const Vector2 b) { return { a.x - b.x, a.y - b.y }; }
inline Vector2 operator+(const Vector2 a, const Vector2 b) { return { a.x + b.x, a.y + b.y }; }


inline bool intersectRay(Vector2 p, Vector2 a, Vector2 b) {
    if ((a.y > p.y) == (b.y > p.y)) return false;
    float x = a.x + (p.y - a.y) * (b.x - a.x) / (b.y - a.y);
    return x > p.x;
}

inline bool intersectBezierRay(Vector2 p, Vector2 a, Vector2 control, Vector2 b) {
    float A = a.y - 2.0f * control.y + b.y;
    float B = 2.0f * (control.y - a.y);
    float C = a.y - p.y;
    float D = B * B - 4.0f * A * C;
    if(D < 0) return false;

    float sqrtD = sqrtf(D);
    float inv2A = 0.5f / A;
    float t = (-B - sqrtD) * inv2A;
    if (t >= 0.0f && t <= 1.0f) {
        float mt = 1.0f - t;
        float x = mt * mt * a.x + 2.0f * mt * t * control.x + t * t * b.x;
        if (x > p.x) return true;
    }

    t = (-B + sqrtD) * inv2A;
    if (t >= 0.0f && t <= 1.0f) {
        float mt = 1.0f - t;
        float x = mt * mt * a.x + 2.0f * mt * t * control.x +  t * t * b.x;
        if (x > p.x) return true;
    }

    return false;
}


inline float distanceToLineSquared(Vector2 p, Vector2 a, Vector2 b) {
    Vector2 ab = b - a;
    Vector2 ap = p - a;
    float ab2 = ab.x * ab.x + ab.y * ab.y;

    float t = (ap.x * ab.x + ap.y * ab.y) / ab2;
    t = std::clamp(t, 0.0f, 1.0f);
    Vector2 closest = a + ab * t;

    float dx = p.x - closest.x;
    float dy = p.y - closest.y;
    return dx * dx + dy * dy;
}


inline float distanceToBezierSquared(Vector2 p, Vector2 a, Vector2 c, Vector2 b) {
     Vector2 A = a - 2.0f*c + b;
    Vector2 B = 2.0f*(c - a);
    Vector2 C = a - p;

    auto eval = [&](float t)
    {
        Vector2 q = A*t*t + B*t + C;
        Vector2 dq = 2.0f*A*t + B;

        return q.x*dq.x + q.y*dq.y;
    };

    auto evalDer = [&](float t) {
        Vector2 q  = A*t*t + B*t + C;
        Vector2 dq = 2.0f*A*t + B;
        Vector2 ddq = 2.0f*A;
        return dq.x*dq.x + dq.y*dq.y + q.x*ddq.x + q.y*ddq.y;
    };


    float best = FLT_MAX;
    float roots[3] = {0, 0.5f, 1};
    for(int i=0;i<3;i++) {
        float t = roots[i];
        for(int j=0;j<5;j++) {
            float f  = eval(t);
            float df = evalDer(t);

            if(fabs(df)<1e-8f) break;

            t -= f/df;

            if(t<0) t=0;
            if(t>1) t=1;
        }
        Vector2 q = a*(1-t)*(1-t) + c*(2*(1-t)*t) +  b*t*t;

        Vector2 d = p-q;
        float dist2 = d.x*d.x+d.y*d.y;
        if(dist2<best) best=dist2;
    }
    Vector2 da=p-a;
    Vector2 db=p-b;
    best = std::min(best, da.x*da.x+da.y*da.y);
    best = std::min(best, db.x*db.x+db.y*db.y);
    return best;
}