#include "AndroidFilter.h"
#include <sys/time.h>

AndroidFilter::AndroidFilter()
{
	
}

//AndroidFilter::~AndroidFilter(){}


void AndroidFilter::setFullscreen(const bool fullscreen)
{
	
}

void AndroidFilter::setSize(int width, int height)
{
	this->width = width;
	this->height = height;
}

Rect AndroidFilter::getInnerSize()
{
	return Rect(width, height);
}

Rect AndroidFilter::getCenter()
{
	return Rect(centerX, centerY);
}

void AndroidFilter::init(const char* windowName, int width, int height, const bool fullscreen)
{
	
}

void AndroidFilter::broadcast()
{
	
}



bool AndroidFilter::getVSync()
{
	return vsync;
}

void  AndroidFilter::setVSync(const bool vsync)
{
	this->vsync = vsync;
}

void AndroidFilter::setCursorPosWithoutMoving()
{
	
}