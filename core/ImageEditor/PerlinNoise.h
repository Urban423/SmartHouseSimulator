#pragma once
#include <umath.h>

TextureStruct PerlinNoise(const int width, const int height, double scale, int octaves, double persistence, unsigned int seed = 42);