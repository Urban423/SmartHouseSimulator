#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Quaternion.h"
#include "Matrix4x4.h"

#define RGB_TO_INT(r, g, b) ((r << 16) + (g << 8) + (b))

struct Vertex
{
	Vector3 pos;
	Vector3 normal;
	Vector2 uv;
};

struct Mesh
{
	int* index;
	int index_size;
	
	Vertex* vertex;
	int vertex_size;
	
	unsigned int* materials;
	unsigned int  number_of_materials;
};

struct TextureStruct
{
	int width;
	int height;
	int* pixels;
};

bool checkClockwise(Vector2 a, Vector2  b, Vector2 c);
bool inTrinagle(Vector2 a, Vector2  b, Vector2 c, Vector2 checkPoint);
void TriangulatePolygon(
	Vertex* vertex_array, Vector3 normal,
	Vector2* vector2_buffer, bool* removed_vertexes,
	int* index_array, int number_of_points,
	int* result_indices);

void freeMesh(Mesh& mesh);



inline float clamp(float a, float b, float value) {
	if(value < a) {
		return a;
	}
	if(value > b) {
		return b;
	}
	return value;
}


inline float Q_rsqrt( float number )
{	
	const float x2 = number * 0.5F;
	const float threehalfs = 1.5F;

	union {
		float f;
		unsigned int i;
	} conv = {number}; // member 'f' set to value of 'number'.
	conv.i = 0x5f3759df - ( conv.i >> 1 );
	conv.f *= threehalfs - x2 * conv.f * conv.f;
	return conv.f;
}





class InputEventSystemI {
public:
	virtual void setSize(int width, int height) = 0;
	virtual void handleMouseClickDown(float x, float y) = 0;
	virtual void handleMouseClickUp(float x, float y) = 0;
	virtual void handleMouseWheel(char delta) = 0;
	virtual void handleKeyDown(int key) = 0;
};


class AppI {
public:
	virtual void enterResizingMode() = 0;
	virtual void  exitResizingMode() = 0;
};