#pragma once
#include <math.h>

static inline void swapInt(int& x, int& y) {
	int temp = x;
	x = y; y = temp;
}

void drawLine(int x1, int y1, int z1, int x2, int y2, int z2, int color, int* buffer, float* z_buffer, int width, int height)
{
	bool changed = 0;
	int deltaX = abs(x2 - x1);
	int deltaY = abs(y2 - y1);
	if (deltaX < deltaY) {
		swapInt(x1, y1);
		swapInt(x2, y2);
		swapInt(deltaX, deltaY);
		changed = 1;
	}
	if (x1 > x2) { 
		swapInt(x1, x2);
		swapInt(y1, y2);
		swapInt(z1, z2);
	}
	if (x2 < 0) { return; }
	if(changed) {
		if (x1 >= height) { return; }
	}
	else {
		if (x1 >= width) { return; }
	}

	//point1
	float dy = (float)(y2 - y1) / deltaX;
	if (y1 < 0)			{ x1 += -y1 / dy; y1 = 0; }
	if (changed) { //if (y1 >= width) { x1 += y1 / dy; 	y1 = width - 1; }
	}
	else {
		if (y1 >= height) { x1 -= (y1 - height) / dy; y1 = height - 1; }
	}
	if (x1 < 0) {  y1 += -x1 * dy; x1 = 0; }
	float y = y1;
	float z = z1;
	float dz = (float)(z2 - z1) / deltaX;



	//point2
	if (y2 < 0) { x2 -= y2 / dy; }
	if (changed) {
		if (y2 >= width)  { x2 -= (y2 - width) / dy; }
		if (x2 >= height) { x2 = height - 1; }
	}
	else {
		if (y2 >= height) { x2 -= (y2 - height) / dy; }
		if (x2 >= width)  { x2 = width - 1; }
	}


	if (changed) {
		for (int x = x1; x < x2; x++, y += dy, z += dz) {
			int index = x * width + (int)y;
			if (z > z_buffer[index]) {
				buffer[index] = color;
			}
		}
	}
	else {
		for (int x = x1; x < x2; x++, y += dy, z += dz) {
			int index = (int)y * width + x;
			if (z > z_buffer[index]) {
				buffer[index] = color;
			}
		}
	}
}
