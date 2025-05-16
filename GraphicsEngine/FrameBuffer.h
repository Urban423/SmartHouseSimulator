#pragma once


class FrameBuffer
{
public:
	FrameBuffer() 														{ init(); }
	FrameBuffer(int width, int height): width(width), height(height) 	{ init(); }
	~FrameBuffer() 														{ destroy(); }
	
	unsigned int getColorTexture() const { return colorTexture; }
	
	void bind();
	void unbind();
	void resize(int width, int height);
private:
	void init();
	void destroy();
private:
	unsigned int fbo = 0;
	unsigned int colorTexture = 0;
	
	int width;
	int height;
};
