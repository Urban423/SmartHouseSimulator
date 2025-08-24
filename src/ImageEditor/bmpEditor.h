#pragma once
#include <stdio.h>



struct TextureStruct
{
	int width;
	int height;
	int* pixels;
};

void writeBMP(const char* filename, TextureStruct texture);
TextureStruct readBMP(const char* filename);

TextureStruct readBMP(const char* filename) {
	FILE* f = fopen(filename, "rb");
	if(f == nullptr) {
		return TextureStruct();
	}
    int er;

	short  bfType;
	int    bfSize;
	short  bfReserved1;
	short  bfReserved2;
	int    bfOffBits;
    er = fread(&bfType, sizeof(short), 1, f);
    er = fread(&bfSize, sizeof(int), 1, f);
    er = fread(&bfReserved1, sizeof(short), 1, f);
    er = fread(&bfReserved2, sizeof(short), 1, f);
    er = fread(&bfOffBits, sizeof(int), 1, f);
	
	
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
	
	er = fread(&biSize, sizeof(int), 1, f);
    er = fread(&biWidth, sizeof(int), 1, f);
    er = fread(&biHeight, sizeof(int), 1, f);
    er = fread(&biPlanes, sizeof(short), 1, f);
    er = fread(&biBitCount, sizeof(short), 1, f);
    er = fread(&biCompression, sizeof(int), 1, f);
    er = fread(&biSizeImage, sizeof(int), 1, f);
    er = fread(&biXPelsPerMeter, sizeof(int), 1, f);
    er = fread(&biYPelsPerMeter, sizeof(int), 1, f);
    er = fread(&biClrUsed, sizeof(int), 1, f);
    er = fread(&biClrImportant, sizeof(int), 1, f);
	
	//move to main data
	fseek(f, bfOffBits, SEEK_SET);
	
	//set texture's data
	TextureStruct texture;
	texture.width = biWidth;
	texture.height = biHeight;
    texture.pixels = (int*)malloc(biWidth * biHeight * sizeof(int));
	
	//helping values
	char temp = 0;
	unsigned int index = 0;
	
    for(int y = 0; y < biHeight; y++) 
	{
		// left_bottom_corner index -> left_to_corner index
		index = (biHeight - y - 1) * biWidth;
		
        for(int x = 0; x < biWidth; x++) 
		{
			int r;
			er = fread(&r, biBitCount / 8, 1, f);
			texture.pixels[index] = r;
			
			//change indexes
			index++;
        }
		
		//skip offset
		fseek(f,  (biWidth * (biBitCount / 8)) % 4, SEEK_CUR);
    }
	fclose(f);
	
	return texture;
}



void writeBMP(const char* filename, TextureStruct texture) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        perror("Error opening file for writing");
        return;
    }

    // BMP file header
    unsigned char header[54] = {
        'B', 'M',  // Signature
        0, 0, 0, 0,  // File size (will fill in later)
        0, 0, 0, 0,  // Reserved
        54, 0, 0, 0,  // Offset to pixel data
        40, 0, 0, 0,  // DIB header size
        0, 0, 0, 0,  // Width (will fill in later)
        0, 0, 0, 0,  // Height (will fill in later)
        1, 0, 24, 0,  // Planes and bits per pixel
        0, 0, 0, 0, 0, 0, 0, 0,  // Compression, image size (0 = none)
        0, 0, 0, 0,  // Resolution, colors in palette, important colors
        0, 0, 0, 0
    };

    // Calculate the file size and fill in the header
    int row_size = (3 * texture.width + 3) & (~3); // Rows must be a multiple of 4 bytes
    int image_size = row_size * texture.height;
    int file_size = 54 + image_size;
    header[2] = (file_size & 0xFF);
    header[3] = ((file_size >> 8) & 0xFF);
    header[4] = ((file_size >> 16) & 0xFF);
    header[5] = ((file_size >> 24) & 0xFF);
    header[18] = (texture.width & 0xFF);
    header[19] = ((texture.width >> 8) & 0xFF);
    header[20] = ((texture.width >> 16) & 0xFF);
    header[21] = ((texture.width >> 24) & 0xFF);
    header[22] = (texture.height & 0xFF);
    header[23] = ((texture.height >> 8) & 0xFF);
    header[24] = ((texture.height >> 16) & 0xFF);
    header[25] = ((texture.height >> 24) & 0xFF);

    fwrite(header, sizeof(unsigned char), 54, file); // Write header

    // Write pixel data
    for (int i = texture.height - 1; i >= 0; i--) {
        for (int j = 0; j < texture.width; j++) {
            int rgb = texture.pixels[i * texture.width + j];
            unsigned char bgr[3] = {
                (unsigned char)(rgb & 0xFF),          // Blue
                (unsigned char)((rgb >> 8) & 0xFF),   // Green
                (unsigned char)((rgb >> 16) & 0xFF)  // Red
            };
            fwrite(bgr, sizeof(unsigned char), 3, file);
        }
    }

    fclose(file);
}