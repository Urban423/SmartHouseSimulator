#include "IndexArrayObject.h"
#include "GraphicConfig.h"
#include <malloc.h>



IndexArrayObject::IndexArrayObject(const IndexArrayDesc& desc)
{
	init(desc);
}

void IndexArrayObject::init(const IndexArrayDesc& desc)
{
	this->number_of_materials 	= desc.number_of_materials;
	this->material_sizes 		= desc.indices_per_material;
	this->size 					= desc.size;
	glGenBuffers(1, &indexBufferID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, desc.size * sizeof(unsigned int), desc.indices, GL_STATIC_DRAW); 
	
	
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

//IndexArrayObject::~IndexArrayObject(){	}

unsigned int IndexArrayObject::getID()
{
	return indexBufferID;
}

unsigned int IndexArrayObject::getNumberOfMaterials()
{
	return this->size;
}

unsigned int IndexArrayObject::getMaterialSize(unsigned int index)
{
	return this->material_sizes[index];
}