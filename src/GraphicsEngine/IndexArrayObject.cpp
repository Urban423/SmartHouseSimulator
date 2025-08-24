#include "IndexArrayObject.h"
#include "GraphicConfig.h"
#include <malloc.h>

IndexArrayObject::IndexArrayObject(const IndexArrayDesc& desc): number_of_materials(desc.number_of_materials), material_sizes(desc.indices_per_material), size(desc.size) { 
	init(desc);
}

void IndexArrayObject::init(const IndexArrayDesc& desc) {
	
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, desc.size * sizeof(unsigned int), desc.indices, GL_STATIC_DRAW); 
	
	
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}