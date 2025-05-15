#pragma once
#include "umath.h"

class Texture
{
public:
	Texture(const TextureStruct desc) { init(desc); }
	void init(const TextureStruct desc);
	
	unsigned int getID();
public:
	unsigned int textureID;
	
	
	unsigned int width;
	unsigned int height;
};