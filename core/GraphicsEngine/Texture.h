#pragma once
#include "umath.h"

class Texture
{
public:
	Texture(const TextureStruct desc) { init(desc); }
	void init(const TextureStruct desc);
	
	inline std::pair<int, int> getSize() { return {width, height}; }
	inline unsigned int getID() { return textureID; }
public:
	unsigned int textureID;
	unsigned int width;
	unsigned int height;
};