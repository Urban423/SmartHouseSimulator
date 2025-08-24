#pragma once
//#include <memory>
#include "Vector3.h"
#include "Vector4.h"


typedef struct VertexBufferDesc
{
	void* verticlesList = nullptr;
	unsigned int vertexSize = 0;
	unsigned int listSize = 0;
}VertexBufferDesc;



class VertexArrayObject
{
public:
	VertexArrayObject(const VertexBufferDesc& desc);
	void init(const VertexBufferDesc& desc);
	~VertexArrayObject();
	
	unsigned int getID();
	unsigned int getVertexSize();
private:
	unsigned int vertexBufferID;
	VertexBufferDesc vertexBufferData;
};


//typedef std::shared_ptr<VertexArrayObject> VertexArrayObjectPtr;
