#include "VertexArrayObject.h"
#include <malloc.h>
#include "Vector3.h"
#include "GraphicConfig.h"

VertexArrayObject::VertexArrayObject(const VertexBufferDesc& desc)
{
	init(desc);
}

void VertexArrayObject::init(const VertexBufferDesc& desc)
{
	glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, desc.vertexSize * desc.listSize, desc.verticlesList, GL_STATIC_DRAW);

    // Here, no need to set vertex attribute pointers yet.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

VertexArrayObject::~VertexArrayObject()
{

}

unsigned int VertexArrayObject::getID()
{
	return vertexBufferID;
}

unsigned int VertexArrayObject::getVertexSize()
{
	return vertexBufferData.vertexSize;
}