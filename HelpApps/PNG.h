#pragma once

typedef struct {
    int width;
    int height;
    int size;
    int gl_color_format;
    int* data;
} RawImageData;


RawImageData readResource(const char* filename);