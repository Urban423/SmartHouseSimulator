#pragma once
#include "Vector2.h"
#include "string.h"

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2


//input
struct WindowInputs
{
	unsigned char* 	 states = nullptr;
	Vector2*		 poses  = nullptr;
	Vector2*		 oldPoses  = nullptr;
	
	unsigned int number = 0;
};




//files
class CFile
{
	friend char readCFile(void* value, int value_size, CFile& file);
    friend void seekCFile(CFile& file, int offset, int origin);
public:
	CFile();
	CFile(char* ptr, int size);
	~CFile();
	
	char* getPtr();
	bool isEmpty();
private:
	char* start = nullptr;
	char* pointer = nullptr;
	unsigned int size = 0;
};


CFile openCFile(const char* name);
char readCFile(void* value, int value_size, CFile& file);
void seekCFile(CFile& file, int offset, int origin);