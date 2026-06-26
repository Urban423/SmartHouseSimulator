#pragma once
#include <string.h>
#include <vector>
#include <TextureManager.h>

#include "Line.h"
#include "Layer.h"

class Image
{
public:
	Image(int width, int height): width(width), height(height) { }
	//~Image() {};
	
	inline int  getWidth()  { return width; }
	inline int  getHeight() { return height; }
	inline void addLayer(Layer* layer) { layers.push_back(layer); }

	unsigned int convertToTexture();
	char saveImage(const char* filename);
private:
	int width;
	int height;
	std::vector<Layer*> layers;
};