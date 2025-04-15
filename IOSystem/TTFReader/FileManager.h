#pragma once
#include "string.h"
#include <stdio.h>
#include <cstdlib>

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2


typedef unsigned int 	uint32;
typedef unsigned short	uint16;
typedef unsigned char	uint8;
typedef int				fixed;
typedef short			fword;
typedef long long		longDateTime;

//files
class CFile
{
	friend char readFile(void* value, int value_size, CFile* file);
    friend void seekFile(CFile* file, int offset, int origin);
public:
	CFile();
	CFile(char* ptr, int size);
	~CFile();
	
	char* getFileStart();
	char* getPtr();
	bool isEmpty();
private:
	char* start = nullptr;
	char* pointer = nullptr;
	unsigned int size = 0;
};


CFile* openFile(const char* name);
char readFile(void* value, int value_size, CFile* file);
void seekFile(CFile& file, int offset, int origin);
void closeFile(CFile* file);
uint32 inverseRead32(CFile* file);
uint16 inverseRead16(CFile* file);