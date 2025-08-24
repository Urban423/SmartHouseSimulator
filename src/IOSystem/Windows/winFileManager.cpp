#include "../IOstructures.h"
#include <stdio.h>
#include <cstdlib>

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

CFile openCFile(const char* name)
{
	//get file full name
	const char* folderName = "./Assets/";
	char* fullName = combineTwoWords(folderName, name);
	FILE* f = fopen(fullName, "rb");
	delete[] fullName;
	
	//end if file not found
	if(f == NULL) {
		return CFile();
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
	
	return CFile(data, size + 1);
}