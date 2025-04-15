#pragma once
#include "Rect.h"
#include "Matrix4x4.h"
#include "Color.h"
#include "VertexArrayObject.h"
#include "IndexArrayObject.h"
#include "FrameBuffer.h"
#include "Shader.h"
#include "Texture.h"

enum CullMode
{
	BackFace = 0,
	FrontFace,
	Both
};

namespace  GraphicsEngine
{
	bool init();
	void enable3D();
	void disable3D();
	void clear(const Color& color);
	void clearDepthBuffer();
	void clearColorDepthBuffer();
	void setViewPort(const unsigned int xOffset, const unsigned int yOffset, const unsigned int width, const unsigned int height);
	void setIndexArrayObject(IndexArrayObject* iao);
	void setVertexArrayObject(VertexArrayObject* vao);
	void setShaderProgram(Shader* program);
	void setTexture(Texture* texture, Shader* shader);
	
	void setVector4(Shader* shader, Vector4 vector4);
	void setMatrix(Shader* shader, Matrix4x4& matrix);
	void setProjectionMatrix(Shader* shader, Matrix4x4& matrix);
	void setCameraViewMatrix(Shader* shader, Matrix4x4& matrix);
	void drawTriangles(unsigned int vertexCount, void* offset);
	
	void setCullMode(const CullMode& mode);
	
	
	VertexArrayObject* createVertexArrayObject(const VertexBufferDesc& desc); 
	IndexArrayObject* createIndexArrayObject(const IndexArrayDesc& desc);
	Shader* createShaderProgram(const ShaderDesc& desc);
	Texture* createTexture(const TextureStruct desc);
};

