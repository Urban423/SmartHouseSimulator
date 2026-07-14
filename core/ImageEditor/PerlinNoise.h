#pragma once
#include <umath.h>
#include "Mesh.h" 
#include "stdlib.h"

TextureStruct PerlinNoise(const int width, const int height, double scale, int octaves, double persistence, float amplitude = 1.0f, unsigned int seed = 42);
Mesh PerlinNoiseMesh(const int width, const int height, double scale, int octaves, double persistence, float amplitude = 1.0f, unsigned int seed = 42);