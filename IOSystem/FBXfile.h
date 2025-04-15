#pragma once
#include "IOstructures.h"
#include <vector>

struct AllProps
{
	int parametr;
	int ArrayLength;
	
	char 	Byte;
	short  	Short;
	int 	Integer;
	float 	Float;
	double 	Double;
	long 	Long;
	
	char* 	rawData;
	short*  ShortArray;
	int* 	IntegerArray;
	float* 	FloatArray;
	double* DoubleArray;
	long* 	LongArray;
};

struct Node
{
	int EndOffset;
	int numProperties;
	int propertyListLen;
	char listName;
	
	char* name;
	std::vector<AllProps> props;
	std::vector<Node*> children;
};




class FBXfile
{
public:
	FBXfile(CFile& file);
	~FBXfile();
	
	unsigned char* readArray(int& ArrayLength, int typeSize, CFile& file);
	AllProps getProperties(char type, CFile& file);
	int readFBXNode(CFile& file, int offset, Node* parent);
	Node* findChildrenByName(const char* name, Node* node);
	Node* getRoot();
private:
	Node root;
};