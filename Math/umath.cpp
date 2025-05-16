#include "umath.h"




void freeMesh(Mesh& mesh)
{
	if(mesh.index != nullptr)
	{
		delete[] mesh.index;
	}
	if(mesh.vertex != nullptr)
	{
		delete[] mesh.vertex;
	}
	if(mesh.materials != nullptr)
	{
		delete[] mesh.materials;
	}
}