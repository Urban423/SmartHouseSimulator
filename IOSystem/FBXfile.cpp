#include "FBXfile.h"
#include <zlib.h>




bool Check2Strings(char* a, char* b)
{
	while(1)
	{
		if(*a != *b)
		{
			return false;
		}
		if(*a == 0)
		{
			break;
		}
		a++;
		b++;
	}
	return true;
}





FBXfile::FBXfile(CFile& file)
{
	
	
	//head
	char Kaydara_FBX_Binary[22] = { 0 };
	readCFile(Kaydara_FBX_Binary, 21, file);
	
	char unknownByte1;
	char unknownByte2;
	readCFile(&unknownByte1, 1, file);
	readCFile(&unknownByte2, 1, file);
	
	int version = 0;
	readCFile(&version, 4, file);
	
	int offset = 27;
	
	do
	{
		Node* node = new Node();
		offset += readFBXNode(file, offset, node);
		if(node->EndOffset == 0) {break;}
		root.children.push_back(node);
	}
	while(1);
}

void deleteNode(Node* node)
{
	delete[] node->name;
	for(int i = 0; i < node->props.size(); i++)
	{
		switch(node->props[i].parametr) {
			case(6):
			{
				delete[] node->props[i].rawData;
				break;
			}
			case(8):
			{
				delete[] node->props[i].IntegerArray;
				break;
			}
			case(10):
			{
				delete[] node->props[i].DoubleArray;
				break;
			}
		}
	}
	for(int i = 0; i < node->children.size(); i++)
	{
		deleteNode(node->children[i]);
	}
}


FBXfile::~FBXfile()
{
	//delete[] root.name;
	for(int i = 0; i < root.props.size(); i++)
	{
		switch(root.props[i].parametr) {
			case(6):
			{
				delete[] root.props[i].rawData;
				break;
			}
			case(8):
			{
				delete[] root.props[i].IntegerArray;
				break;
			}
			case(10):
			{
				delete[] root.props[i].DoubleArray;
				break;
			}
		}
	}
	for(int i = 0; i < root.children.size(); i++)
	{
		deleteNode(root.children[i]);
	}
}


Node* FBXfile::findChildrenByName(const char* name, Node* node)
{
	if(node == nullptr){return nullptr;}
	char* sName = const_cast<char*>(name);
	for(int i = 0; i < node->children.size(); i++)
	{
		if(Check2Strings(node->children[i]->name, sName))
		{
			return node->children[i];
		}
	}
	return nullptr;
}

Node* FBXfile::getRoot()
{
	return &root;
}

unsigned char* FBXfile::readArray(int& ArrayLength, int typeSize, CFile& file)
{
	int Encoding = 0;
	int CompressedLength = 0;
	readCFile(&ArrayLength, sizeof(int), file);
	readCFile(&Encoding, sizeof(int), file);
	readCFile(&CompressedLength, sizeof(int), file);
	
	unsigned char* array = new unsigned char[ArrayLength * typeSize];
	if(Encoding == 0)
	{
		readCFile(array, ArrayLength * typeSize, file);
	}
	else
	{
		long unsigned int lLength = ArrayLength * typeSize;
		unsigned char* compressArray = new unsigned char[CompressedLength];
		readCFile(compressArray, CompressedLength, file);
		uncompress(array, &lLength, compressArray, CompressedLength);
		
		if(compressArray != nullptr)
		{
			delete[] compressArray;
		}
	}
	return array;
}


AllProps FBXfile::getProperties(char type, CFile& file)
{
	AllProps prop;
	switch(type)
	{
		case('C'):
		{
			readCFile(&prop.Byte, sizeof(char), file);
			prop.parametr = 0;
			return prop;
		}
		case('Y'):
		{
			readCFile(&prop.Short, sizeof(short), file);
			prop.parametr = 1;
			return prop;
		}
		case('I'):
		{
			readCFile(&prop.Integer, sizeof(int), file);
			prop.parametr = 2;
			return prop;
		}
		case('F'):
		{
			readCFile(&prop.Float, sizeof(int), file);
			prop.parametr = 3;
			return prop;
		}
		case('D'):
		{
			readCFile(&prop.Double, sizeof(double), file);
			prop.parametr = 4;
			return prop;
		}
		case('L'):
		{
			readCFile(&prop.Long, sizeof(long), file);
			prop.parametr = 5;
			return prop;
		}
		
		
		
		
		case('R'):
		{
			
		}
		case('S'):
		{
			readCFile(&prop.ArrayLength, sizeof(int), file);
			prop.rawData = new char[prop.ArrayLength];
			readCFile(prop.rawData, prop.ArrayLength, file);
			prop.parametr = 6;
			return prop;
		}
		
		
		
		
		case('b'):
		{
			prop.parametr = 7;
			break;
		}
		case('i'):
		{
			prop.IntegerArray = (int*)readArray(prop.ArrayLength, sizeof(int), file);
			prop.parametr = 8;
			return prop;
		}
		case('f'):
		{
			prop.parametr = 9;
			break;
		}
		case('d'):
		{
			
			prop.DoubleArray = (double*)readArray(prop.ArrayLength, sizeof(double), file);
			prop.parametr = 10;
			return prop;
		}
		case('l'):
		{
			prop.parametr = 11;
			break;
		}
	}
	return prop;
}


int FBXfile::readFBXNode(CFile& file, int offset, Node* parent)
{
	seekCFile(file, offset, SEEK_SET);
	
	int bytesRead = 13;
	
	
	//read head
	readCFile(&parent->EndOffset, sizeof(int), file);
	readCFile(&parent->numProperties, sizeof(int), file);
	readCFile(&parent->propertyListLen, sizeof(int), file);
	readCFile(&parent->listName, sizeof(char), file);
	parent->name = new char[parent->listName + 1];
	parent->name[parent->listName] = 0;
	readCFile(parent->name, parent->listName, file);
	bytesRead += parent->listName;
	
	
	//read properrties
	for(int i = 0; i < parent->numProperties; i++)
	{
		char type = 0;
		readCFile(&type, 1, file);
		parent->props.push_back(getProperties(type, file));
	}
	bytesRead += parent->propertyListLen;
	
	
	//read children
	while(bytesRead + offset < parent->EndOffset)
	{
		Node* child = new Node();
		bytesRead += readFBXNode(file, offset + bytesRead, child);
		if(child->EndOffset == 0) {break;}
		parent->children.push_back(child);
	}
	
	
	return bytesRead;
}
