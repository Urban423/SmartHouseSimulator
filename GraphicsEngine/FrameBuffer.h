#pragma once


class FrameBuffer
{
public:
	FrameBuffer();
	~FrameBuffer();
	
	
	void bind();
	void unbind();
	void resize();
private:
	int width;
	int height;
};
