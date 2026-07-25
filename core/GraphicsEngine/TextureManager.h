#pragma once
#include "Texture.h"
#include "FrameBuffer.h"

class TextureManager {
public:
	static unsigned int GetTextureByID		(unsigned int id);
	static unsigned int	CreateTexture 		(const TextureStruct desc);
	static unsigned int	CreateFrameBuffer	(int width, int height);
	static void			DeleteTexture		(unsigned int id);
	static void 		SetRenderTarget		(unsigned int id);
	static void 		ResizeFrameBuffer	(unsigned int id, int width, int height);
	static std::pair<int, int> GetTextureSize(unsigned int id);
private:
	static TextureManager* 		textureManager;
	std::stack<unsigned int>	freeTextureIDs;
	std::stack<unsigned int>	freeFramebufferIDs;
	
	std::vector<Texture*>		textures;
	std::vector<FrameBuffer*>	frameBuffers;
};