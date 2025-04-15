#include <malloc.h>
#include <stdio.h>
#include "zlib.h"

typedef struct Texture
{
	int* pixels;
	unsigned int width;
	unsigned int height;
}Texture;

char ReadBMPFile(Texture* texture, const char* filename)
{
    FILE *f = fopen(filename, "rb");
    if( !f ) return 1;

	short  bfType;
	int    bfSize;
	short  bfReserved1;
	short  bfReserved2;
	int    bfOffBits;
    fread(&bfType, sizeof(short), 1, f);
    fread(&bfSize, sizeof(int), 1, f);
    fread(&bfReserved1, sizeof(short), 1, f);
    fread(&bfReserved2, sizeof(short), 1, f);
    fread(&bfOffBits, sizeof(int), 1, f);
	
	
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
	
	fread(&biSize, sizeof(int), 1, f);
    fread(&biWidth, sizeof(int), 1, f);
    fread(&biHeight, sizeof(int), 1, f);
    fread(&biPlanes, sizeof(short), 1, f);
    fread(&biBitCount, sizeof(short), 1, f);
    fread(&biCompression, sizeof(int), 1, f);
    fread(&biSizeImage, sizeof(int), 1, f);
    fread(&biXPelsPerMeter, sizeof(int), 1, f);
    fread(&biYPelsPerMeter, sizeof(int), 1, f);
    fread(&biClrUsed, sizeof(int), 1, f);
    fread(&biClrImportant, sizeof(int), 1, f);
	
	//move to main data
	fseek(f, bfOffBits, SEEK_SET);
	
	//set texture's data
	char tmp;
	texture->width = biWidth;
	texture->height = biHeight;
    texture->pixels = (int*)malloc(biWidth * biHeight * sizeof(int));
	
	char temp = 0;
	unsigned int index = 0;
	
    for(int y = 0; y < biHeight; y++) 
	{
		// left_bottom_corner index -> left_to_corner index
		index = y * biWidth;
		
        for(int x = 0; x < biWidth; x++) 
		{
			if(biBitCount == 32)
			{
				fread(&texture->pixels[index], 4, 1, f);
			}
			else
			{
				fread(&texture->pixels[index], biBitCount / 8, 1, f);
			}
			
			char* t = (char*)&texture->pixels[index];
			char temp = t[0];
			temp = t[2];
			t[2] = t[0];
			t[0] = temp;
			
			//change indexes
			index++;
        }
		
		//skip offset
		fseek(f,  (biWidth* (biBitCount / 8)) % 4, SEEK_CUR);
    }
    return 0;
}

void saveTexture(Texture* texture, const char* filename)
{
	FILE* file = fopen(filename, "wb");
	fwrite(&texture->width, 4, 1, file);
	fwrite(&texture->height, 4, 1, file);
	for(int i = 0; i < texture->width * texture->height; i++)
	{
		int a = texture->pixels[i];
		fwrite(&texture->pixels[i], 4, 1, file);
	}
	fclose(file);
}

int main()
{
	Texture tex;
	
	ReadBMPFile(&tex, "../Assets/test.bmp");
	saveTexture(&tex, "../Assets/test.b");
	
	long unsigned int size;
	unsigned char* buffer;
	long unsigned int size_or = tex.width * tex.height * 4;
	compress(buffer, &size, (const unsigned char*)tex.pixels, size_or);
	printf("%d %d\n", size_or, size);
	
	ReadBMPFile(&tex, "../Assets/test.bmp");
	//saveTexture(&tex, "../IOSystem/Android/build/apk/assets/test.b");
	
	return 0;
}