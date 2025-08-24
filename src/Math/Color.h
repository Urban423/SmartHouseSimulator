#pragma once

class Color
{
public:
	Color();
	Color(float r, float g, float b);
	Color(float r, float g, float b, float a);
	Color(const Color& color);
	//~Color();
public:
	float r = 0;
	float g = 0;
	float b = 0;
	float a = 0;
};