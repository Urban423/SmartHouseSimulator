#pragma once
#include "Vector4.h"

class Color
{
public:
	Color() : r(0), g(0), b(0), a(0xFF) {};
	Color(char light) : r(light), g(light), b(light), a(0xFF) {};
	Color(char r, char g, char b) : r(r), g(g), b(b), a(0xFF) {};
	Color(int r, int g, int b) : r(r), g(g), b(b), a(0xFF) {};
	Color(float r, float g, float b) : a(0xFF)
	{
		this->r = static_cast<unsigned char>(r * 255.0f);
		this->g = static_cast<unsigned char>(g * 255.0f);
		this->b = static_cast<unsigned char>(b * 255.0f);
	};
	Color(double r, double g, double b) : a(0xFF)
	{
		this->r = static_cast<unsigned char>(r * 255.0f);
		this->g = static_cast<unsigned char>(g * 255.0f);
		this->b = static_cast<unsigned char>(b * 255.0f);
	};
	Color(char r, char g, char b, char a) : r(r), g(g), b(b), a(a) {};

	inline Vector4 ToVector4() const
	{
		constexpr float inv = 1.0f / 255.0f;
		return {r * inv, g * inv, b * inv, a * inv};
	}

public:
	unsigned char r = 0;
	unsigned char g = 0;
	unsigned char b = 0;
	unsigned char a = 0xFF;
};