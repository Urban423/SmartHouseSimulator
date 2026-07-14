#pragma once
#include "stdlibInclude.h"

class FrameBuffer
{
public:
	FrameBuffer() 														{ init(); }
	FrameBuffer(int width, int height): width(width), height(height) 	{ init(); }
	~FrameBuffer() 														{ destroy(); }
	
	inline unsigned int getColorTexture() const	{ return colorTexture; }
	inline std::pair<int, int>  getSize() const	{ return {width, height}; }
	
	void bind();
	void unbind();
	void resize(int width, int height);
private:
	void init();
	void destroy();
private:
	unsigned int fbo = 0;
	unsigned int colorTexture = 0;
	
	int width = 1980;
	int height = 1080;
};
