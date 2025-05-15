#pragma once
#include "Texture.h"
#include "FrameBuffer.h"
#include <stack> 
#include <vector> 

class TextureManager {
public:
	static unsigned int GetTextureByID		(unsigned int id);
	static unsigned int	CreateTexture 		(const TextureStruct desc);
	static unsigned int	CreateFrameBuffer	();
	static void			DeleteTexture		(unsigned int id);
private:
	static TextureManager* 		textureManager;
	std::stack<unsigned int>	freeTextureIDs;
	std::stack<unsigned int>	freeFramebufferIDs;
	
	std::vector<Texture*>		textures;
	std::vector<FrameBuffer*>	frameBuffers;
};