#include "FileManager.h"
#include <stdio.h>
#include <cstdlib>

uint32 inverseRead32(CFile* file) {
	uint32 was;
	readFile(&was, sizeof(uint32), file);
	return ((was & 0xFF) << 24) | ((was & 0xFF00) << 8) | ((was & 0xFF0000) >> 8) | ((was & 0xFF'00'00'00) >> 24);
}

uint16 inverseRead16(CFile* file) {
	uint16 was;
	readFile(&was, sizeof(uint16), file);
	return ((was & 0xFF) << 8) | ((was & 0xFF00) >> 8);
}






int getTextSize(const char* text)
{
	int size = 0;
	while(1) {
		if(text[size] == 0) {
			break;
		}
		size++;
	}
	return size;
}

char* combineTwoWords(const char* text1, const char* text2)
{
	int text1_size = getTextSize(text1);
	int text2_size = getTextSize(text2);
	char* summ = new char[text1_size + text2_size + 1];
	int i = 0;
	for(; i < text1_size; i++) {
		summ[i] = text1[i];
	}
	for(int j = 0; j < text2_size; j++, i++) {
		summ[i] = text2[j];
	}
	summ[i] = 0;
	return summ;
}

CFile* openFile(const char* name)
{
	//get file full name
	const char* folderName = "./";
	char* fullName = combineTwoWords(folderName, name);
	FILE* f = fopen(fullName, "rb");
	delete[] fullName;
	
	//end if file not found
	if(f == NULL) {
		return nullptr;
	}
	
	//get file size
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f); 
	fseek(f, 0, SEEK_SET);

	//read file
	char* data = nullptr;
	data = static_cast<char*>(malloc(size + 1));
	fread(data, size, 1, f);
	data[size] = '\0';
	fclose(f);
	
	return new CFile(data, size + 1);
}


void closeFile(CFile* file) {
	if(file != nullptr) {
		delete file;
	}
}





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

char* CFile::getPtr() {
	return pointer;
}

char* CFile::getFileStart() {
	return start;
}

char readFile(void* value, int value_size, CFile* file)
{
	memcpy(value, file->pointer, value_size);
	file->pointer += value_size;
	return 0;
}

void seekFile(CFile* file, int offset, int origin)
{
	switch(origin)
	{
		case(SEEK_SET):
		{
			if(offset < 0)
			{
				offset = 0;
			}
			file->pointer = file->start + offset;
			return;
		}
		case(SEEK_END):
		{
			file->pointer = file->start + file->size + offset;
			return;
		}
		case(SEEK_CUR):
		{
			file->pointer += offset;
			return;
		}
	}
}