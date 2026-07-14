#pragma once
#include <string.h>

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

//files
class CFile
{
	friend CFile createCFile();
    friend void writeCFile(const void* data, int size, CFile& file);
    friend void saveCFile(const char* name, CFile& file);

	friend char readCFile(void* value, int value_size, CFile& file);
    friend void seekCFile(CFile& file, int offset, int origin);
public:
	CFile();
	CFile(char* ptr, int size);
	~CFile();
	
	inline char* getPtr() { return pointer; }
	inline bool isEmpty() { return pointer == nullptr; };
private:
	char* start = nullptr;
	char* pointer = nullptr;
	unsigned int size = 0;
    unsigned int capacity = 0;
};


CFile openCFile(const char* name);
char readCFile(void* value, int value_size, CFile& file);
void seekCFile(CFile& file, int offset, int origin);

CFile createCFile();
void writeCFile(const void* data, int size, CFile& file);
void saveCFile(const char* name, CFile& file);