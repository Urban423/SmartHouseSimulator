#include "Image.h"
#include <Vector2.h>
#include <Rect.h>

void writeLayersToPixelsBuffer(Layer* layer, int imageWidth, int imageHeight, int* buffer) {	
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
			*(ptrImage++) = *(ptrLayer++) | 0xFF'00'00'00;
		}
	}
}

char Image::saveImage(const char* filename) {
	int* buffer = new int[width * height];
	for(int i = 0; i < layers.size(); i++) {
		writeLayersToPixelsBuffer(layers[i], width, height, buffer);
	}
	
	//writeBMP(filename, {width, height, buffer});
	delete[] buffer;
	return 0;
}

unsigned int Image::convertToTexture() {
    int* buffer = new int[width * height];
	// memset(buffer, 0xff'ff'ff'ff, width * height * 4);
	for(int i = 0; i < layers.size(); i++) {
		writeLayersToPixelsBuffer(layers[i], width, height, buffer);
	}
    unsigned int textureID = TextureManager::CreateTexture({width, height, buffer});
    return textureID; 
}
	