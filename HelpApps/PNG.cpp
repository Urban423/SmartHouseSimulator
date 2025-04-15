#include "PNG.h"
#include "AndroidFileManager.h"
#include <malloc.h>

/*
int get_gl_color_format(const int png_color_format) {
    switch (png_color_format) 
	{
        case 4:
            return GL_LUMINANCE;
        case 32:
            return GL_RGBA;
        case 8:
            return GL_LUMINANCE_ALPHA;
    }
 
    return 0;
}
*/

RawImageData readResource(const char* filename)
{
	RawImageData image;
	
	File *f = open(filename, "rb");
    read(&image.width, sizeof(int), 1, f);
    read(&image.height, sizeof(int), 1, f);
	image.data = (int*)malloc(image.width * image.height * sizeof(int));
	
	for(int i = 0; i < image.width * image.height; i++)
	{
		read(&image.data[i], 4, 1, f);
	}
	
	return image;
}