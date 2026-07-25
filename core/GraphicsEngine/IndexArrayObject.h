#pragma once

struct IndexArrayDesc {
	unsigned int* indices = nullptr;
	unsigned int  size = 0;
};


class IndexArrayObject
{
public:
	IndexArrayObject(const IndexArrayDesc& desc);
	void init(const IndexArrayDesc& desc);
	void destroy();
	
	inline unsigned int getID() { return indexBufferID; }
private:
	unsigned int indexBufferID;
};
