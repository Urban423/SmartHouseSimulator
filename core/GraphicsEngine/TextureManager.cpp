#include "TextureManager.h"
#include "GraphicsEngine.h"

#define TYPE_BIT 1 << 31
#define VALID_BIT 1 << 30
#define INDEX_MASK ~(TYPE_BIT | VALID_BIT)
#define TEXTURE 0
#define FRAMEBUFFER 1

TextureManager *TextureManager::textureManager = new TextureManager();

unsigned int MakeID(char type, bool valid, unsigned int index)
{
	return (type ? TYPE_BIT : 0) | (valid ? VALID_BIT : 0) | (index & INDEX_MASK);
}

static char GetType(unsigned int id)
{
	return (id & TYPE_BIT) ? FRAMEBUFFER : TEXTURE;
}

unsigned int GetIndex(unsigned int id)
{
	return id & INDEX_MASK;
}

unsigned int TextureManager::GetTextureByID(unsigned int id)
{
	unsigned int index = GetIndex(id);
	if (GetType(id) == TEXTURE) {
		if (index < textureManager->textures.size())
			return textureManager->textures[index]->getID();
	}
	else
	{
		if (index < textureManager->frameBuffers.size())
			return textureManager->frameBuffers[index]->getColorTexture();
	}
	return textureManager->textures[0]->getID();
}

unsigned int TextureManager::CreateTexture(const TextureStruct desc) {
	unsigned int index;
	if (!textureManager->freeTextureIDs.empty()) {
		index = textureManager->freeTextureIDs.top();
		textureManager->freeTextureIDs.pop();
		// delete textureManager->textures[index];
		textureManager->textures[index] = new Texture(desc);
	}
	else {
		index = static_cast<unsigned int>(textureManager->textures.size());
		textureManager->textures.push_back(new Texture(desc));
	}
	return MakeID(TEXTURE, true, index);
}

unsigned int TextureManager::CreateFrameBuffer(int width, int height)
{
	unsigned int index;
	if (!textureManager->freeFramebufferIDs.empty())
	{
		index = textureManager->freeFramebufferIDs.top();
		textureManager->freeFramebufferIDs.pop();
		delete textureManager->frameBuffers[index];
		textureManager->frameBuffers[index] = new FrameBuffer();
	}
	else
	{
		index = static_cast<unsigned int>(textureManager->frameBuffers.size());
		textureManager->frameBuffers.push_back(new FrameBuffer(width, height));
	}
	return MakeID(FRAMEBUFFER, true, index);
}

void TextureManager::DeleteTexture(unsigned int id)
{
	unsigned int index = GetIndex(id);
	if (GetType(id) == TEXTURE)
	{
		if (index < textureManager->textures.size())
		{
			delete textureManager->textures[index];
			textureManager->textures[index] = nullptr;
			textureManager->freeTextureIDs.push(index);
		}
	}
	else
	{
		if (index < textureManager->frameBuffers.size())
		{
			delete textureManager->frameBuffers[index];
			textureManager->frameBuffers[index] = nullptr;
			textureManager->freeFramebufferIDs.push(index);
		}
	}
}

void TextureManager::SetRenderTarget(unsigned int id)
{
	unsigned int index = GetIndex(id);
	if (GetType(id) == TEXTURE || id == -1)
	{
		GraphicsEngine::setRenderTargetWindow();
	}
	else
	{
		textureManager->frameBuffers[index]->bind();
	}
}

void TextureManager::ResizeFrameBuffer(unsigned int id, int width, int height)
{
	unsigned int index = GetIndex(id);
	if (GetType(id) == TEXTURE || id == -1) {
		return;
	}
	textureManager->frameBuffers[index]->resize(width, height);
}

std::pair<int, int> TextureManager::GetTextureSize(unsigned int id)
{
	unsigned int index = GetIndex(id);
	if (GetType(id) == TEXTURE)
	{
		return textureManager->textures[index]->getSize();
	}
	return textureManager->frameBuffers[index]->getSize();
}