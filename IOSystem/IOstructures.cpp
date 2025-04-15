#include "IOstructures.h"

//input




//files
CFile::CFile(){}

CFile::CFile(char* ptr, int size)
{
	start = ptr;
	pointer = ptr;
	this->size = size;
}

CFile::~CFile()
{
	#ifdef glad
		delete[] start;
	#endif
}

bool CFile::isEmpty()
{
	return pointer == nullptr;
}

char* CFile::getPtr()
{
	return pointer;
}

char readCFile(void* value, int value_size, CFile& file)
{
	memcpy(value, file.pointer, value_size);
	file.pointer += value_size;
	return 0;
}

void seekCFile(CFile& file, int offset, int origin)
{
	switch(origin)
	{
		case(SEEK_SET):
		{
			if(offset < 0)
			{
				offset = 0;
			}
			file.pointer = file.start + offset;
			return;
		}
		case(SEEK_END):
		{
			file.pointer = file.start + file.size + offset;
			return;
		}
		case(SEEK_CUR):
		{
			file.pointer += offset;
			return;
		}
	}
}