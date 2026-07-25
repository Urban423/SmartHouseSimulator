#pragma once
#include "Vector3.h"
#include "Vector4.h"


struct VertexBufferDesc {
	void* verticlesList = nullptr;
	unsigned int vertexSize = 0;
	unsigned int listSize = 0;
};



class VertexArrayObject {
public:
	VertexArrayObject(const VertexBufferDesc& desc);
	void init(const VertexBufferDesc& desc);
	void destroy();
	
	unsigned int getID() { return vertexBufferID;}
private:
	unsigned int vertexBufferID;
};