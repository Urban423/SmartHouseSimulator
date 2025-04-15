#include "Texture.h"
#include "GraphicConfig.h"

Texture::Texture(const TextureStruct desc)
{
	init(desc);
}

void Texture::init(const TextureStruct desc)
{
	width = desc.width;
	height = desc.height;
	
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, desc.width, desc.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, desc.pixels);
	
	glGenerateMipmap(GL_TEXTURE_2D);
	
	
	glBindTexture(GL_TEXTURE_2D, 0);
	
	delete[] desc.pixels;
}

//Texture::~Texture() {}



unsigned int Texture::getID()
{
	return textureID;
}