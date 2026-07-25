#include "IndexArrayObject.h"
#include "GraphicConfig.h"
#include <cstdio>

IndexArrayObject::IndexArrayObject(const IndexArrayDesc& desc) { 
	init(desc);
}

void IndexArrayObject::init(const IndexArrayDesc& desc) {
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, desc.size * sizeof(unsigned int), desc.indices, GL_STATIC_DRAW); 
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexArrayObject::destroy() {
	if (indexBufferID) {
		glDeleteBuffers(1, &indexBufferID);
		indexBufferID = 0;
	}
}