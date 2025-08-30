#pragma once
#include "Rect.h"

class AndroidFilter
{
public:
	AndroidFilter();
	//~AndroidFilter();
	
	void setFullscreen(const bool fullscreen);
	void setSize(int width, int height);
	Rect getInnerSize();
	Rect getCenter();
	
	void setCursorPosWithoutMoving();
	
	void init(const char* windowName, int width, int height, const bool fullscreen);
	void broadcast();
	
	bool getVSync();
	void setVSync(bool vsync);
private:
	bool vsync;

	int width;
	int height;
	int left;
	int top;
	int centerX;
	int centerY;
};
