#include "Matrix3x3.h"
#include "Vector4.h"
#include <math.h>


Matrix3x3::Matrix3x3() {}

//Matrix3x3::~Matrix3x3() {}

void Matrix3x3::setIdentity()
{
	for(int y = 0; y < 3; y++)
	{
		for(int x = 0; x < 3; x++)
		{
			mat[y][x] = 0;
		}
	}
	mat[0][0] = 1;
	mat[1][1] = 1;
	mat[2][2] = 1;
}

float Matrix3x3::getDeterminant()
{
	Vector4 v1 = Vector4(mat[0][0], mat[1][0], mat[2][0], mat[3][0]);
	Vector4 v2 = Vector4(mat[0][1], mat[1][1], mat[2][1], mat[3][1]);
	Vector4 v3 = Vector4(mat[0][2], mat[1][2], mat[2][2], mat[3][2]);
	
	Vector4 v4  = Vector4::cross(v1, v2, v3);
	
	return 
	 -(mat[0][3] * v4.x +
	   mat[1][3] * v4.y +
	   mat[2][3] * v4.z + 
	   mat[3][3] * v4.w);
}

void Matrix3x3::inverse()
{
	Matrix3x3 out;
	float determinant = getDeterminant();
	
	Vector4 vectors[3];
	Vector4 v;
	unsigned int index;
	unsigned int index_to_ignore = 0;
	
	
	for(unsigned int i = 0; i < 4; i++)
	{
		index_to_ignore = i;
		index = 0;
		for(unsigned int j = 0; j < 4; j++)
		{
			if(j != index_to_ignore)
			{
				vectors[index] = Vector4(mat[j][0], mat[j][1], mat[j][2], mat[j][3]);
				index++;
			}
		}
		
		v  = Vector4::cross(vectors[0], vectors[1], vectors[2]);
	
		out.mat[0][i] = (float)pow(-1, i) * v.x / determinant;
		out.mat[1][i] = (float)pow(-1, i) * v.y / determinant;
		out.mat[2][i] = (float)pow(-1, i) * v.z / determinant;
		out.mat[3][i] = (float)pow(-1, i) * v.w / determinant;
	}
	
	
	*this = out;
}


void Matrix3x3::setScale(const Vector2& scale)
{
	mat[0][0] = scale.x;
	mat[1][1] = scale.y;
}


void Matrix3x3::setTranslation(const Vector2& translation)
{
	mat[2][0] = translation.x;
	mat[2][1] = translation.y;
}

void Matrix3x3::setRotation(const float rotation)
{
	
}

float* Matrix3x3::getPtr()
{
	return &mat[0][0];
}


void Matrix3x3::operator *=(const Matrix3x3& matrix)
{
	Matrix3x3 out;
	for(unsigned int i = 0; i < 3; i++)
	{
		for(unsigned int j = 0; j < 3; j++)
		{
			out.mat[i][j] = \
			mat[i][0] * matrix.mat[0][j] +
			mat[i][1] * matrix.mat[1][j] +
			mat[i][2] * matrix.mat[2][j] +
			mat[i][3] * matrix.mat[3][j];
		}
	}
	
	*this = out;
}

float* Matrix3x3::operator[](const unsigned int index) {
	return mat[index];
}