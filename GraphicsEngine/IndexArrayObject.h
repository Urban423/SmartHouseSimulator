#pragma once

struct IndexArrayDesc
{
	unsigned int* indices = nullptr;
	unsigned int  size = 0;
	unsigned int  number_of_materials = 0;
	unsigned int* indices_per_material = nullptr;
};


class IndexArrayObject
{
public:
	IndexArrayObject(const IndexArrayDesc& desc);
	void init(const IndexArrayDesc& desc);
	//~IndexArrayObject();
	
	unsigned int getNumberOfMaterials();
	unsigned int getMaterialSize(unsigned int index);
	unsigned int getID();
private:
	unsigned int  indexBufferID;
	unsigned int  size;
	unsigned int  number_of_materials = 0;
	unsigned int* material_sizes;
};
