#pragma once
#include <string.h>
#include <windows.h>
#include "./Line.h"
#include "./bmpEditor.h"
#include "./Layer.h"
#include <Vector2.h>
#include <Rect.h>

//#define RGB(r, g, b) ((r << 16) + (g << 8) + (b))


class ImageWindow {
public:
	ImageWindow(void* pid, void* piThread): pid(pid), piThread(piThread) {}
	~ImageWindow() {
		TerminateProcess(pid, 0);
		CloseHandle(pid);
		CloseHandle(piThread);
	}
private:
	void* pid;
	void* piThread;
};

class Image
{
public:
	Image(int width, int height);
	~Image() {};
	
	inline int  getWidth()  {return width;}
	inline int  getHeight() {return height;}
	
	ImageWindow showImage();
	char saveImage(const char* filename);
	void addLayer(Layer* layer);
private:
	int width;
	int height;
	std::vector<Layer*> layers;
};

Image::Image(int width, int height): width(width), height(height) { }

ImageWindow Image::showImage() {
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi;

	saveImage("sanyaLox.bmp");

	if (CreateProcess(
		NULL,         // Application name (NULL to use command line)
		(LPSTR)"../../HelpApps//ImageWindow//ImageWindow.exe sanyaLox.bmp", // Command line
		NULL, NULL,   // Security attributes (NULL for default)
		FALSE,        // Do not inherit handles
		0,            // No special creation flags
		NULL,         // Use parent's environment
		NULL,         // Use parent's working directory
		&si, &pi      // Startup and process information
	))
	{
		return ImageWindow(pi.hProcess, pi.hThread); 
	}
	return ImageWindow(NULL, NULL);
}

void writeLayerToBuffer(Layer* layer, int imageWidth, int imageHeight, int* buffer) {	
	int xOffset 	= layer->getOffsetX();
	int yOffset 	= layer->getOffsetY();
	int layerWidth 	= layer->getWidth();
	int layerHeight = layer->getHeight();
	int* ptrImage;
	int* ptrLayer = layer->getPtr();
	int yStart = yOffset;
	int xStart = xOffset;
	int yEnd = yOffset + layerHeight;
	int xEnd = xOffset + layerWidth;
	int layerXStart = 0;
	int layerYStart = 0;
	
	if(yOffset < 0) {
		layerYStart -= yOffset;
		yStart = 0;
	}
	if(xOffset < 0) {
		layerXStart -= xOffset;
		xStart = 0;
	}
	if(yEnd > imageHeight) {
		yEnd = imageHeight;
	}
	if(xEnd > imageWidth) {
		xEnd = imageWidth;
	}
	for(int y = yStart; y < yEnd; y++) {
		ptrImage = buffer + (imageWidth * y + xStart);
		ptrLayer = layer->getPtr() + (layerWidth * layerYStart++) + layerXStart;
		for(int x = xStart; x < xEnd; x++) {
			*(ptrImage++) = *(ptrLayer++);
		}
	}
}

char Image::saveImage(const char* filename) {
	int* buffer = new int[width * height];
	for(int i = 0; i < layers.size(); i++) {
		writeLayerToBuffer(layers[i], width, height, buffer);
	}
	
	writeBMP(filename, {width, height, buffer});
	delete[] buffer;
	return 0;
}

void Image::addLayer(Layer* layer) {
	layers.push_back(layer);
}