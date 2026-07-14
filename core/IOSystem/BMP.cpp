#include "IOSystem.h"

bool IOSystem::readBMP(TextureStruct& out, const char* filename) {
	CFile f = openCFile(filename);
	if(f.isEmpty()) return false;

	short  bfType;
	int    bfSize;
	short  bfReserved1;
	short  bfReserved2;
	int    bfOffBits;
    readCFile(&bfType, sizeof(short), f);
    readCFile(&bfSize, sizeof(int), f);
    readCFile(&bfReserved1, sizeof(short), f);
    readCFile(&bfReserved2, sizeof(short), f);
    readCFile(&bfOffBits, sizeof(int), f);
	
	
	int    biSize;
	int    biWidth;
	int    biHeight;
	short   biPlanes;
	short   biBitCount;
	int    biCompression;
	int    biSizeImage; 
	int    biXPelsPerMeter;
	int    biYPelsPerMeter;
	int    biClrUsed;     
	int    biClrImportant;
	
	readCFile(&biSize, sizeof(int), f);
    readCFile(&biWidth, sizeof(int), f);
    readCFile(&biHeight, sizeof(int), f);
    readCFile(&biPlanes, sizeof(short), f);
    readCFile(&biBitCount, sizeof(short), f);
    readCFile(&biCompression, sizeof(int), f);
    readCFile(&biSizeImage, sizeof(int), f);
    readCFile(&biXPelsPerMeter, sizeof(int), f);
    readCFile(&biYPelsPerMeter, sizeof(int), f);
    readCFile(&biClrUsed, sizeof(int), f);
    readCFile(&biClrImportant, sizeof(int), f);
	
	
	//move to main data
	seekCFile(f, bfOffBits, SEEK_SET);
	
	//set texture's data
	out.width = biWidth;
	out.height = biHeight;
    out.pixels = new int[biWidth * biHeight];
	
	//helping values
	char temp = 0;
	unsigned int index = 0;
	
    for(int y = 0; y < biHeight; y++)  {
		// left_bottom_corner index -> left_to_corner index
		//index = (biHeight - y - 1) * biWidth;
		
        for(int x = 0; x < biWidth; x++)  {
			int r;
			readCFile(&r, biBitCount / 8, f);
			char* bytes = reinterpret_cast<char*>(&r);
			std::swap(bytes[0], bytes[2]);
			if(biBitCount < 32) {
				bytes[3] = 0xff;
			}
			out.pixels[index] = r;
			index++;
        }
		
		//skip offset
		int rowBytes = (biBitCount / 8) * biWidth;
		int padding = (4 - (rowBytes % 4)) % 4;
		seekCFile(f, padding, SEEK_CUR);
    }
	
	return true;
}