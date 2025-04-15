#include "umath.h"


bool checkClockwise(Vector2 a, Vector2  b, Vector2 c)
{
	return 0.5f * (a.x * ((double) c.y - b.y) + b.x * ((double) a.y - c.y) + c.x * ((double) b.y - a.y)) > 0;
}

bool inTrinagle(Vector2 a, Vector2  b, Vector2 c, Vector2 checkPoint)
{
	  float denominator = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);

        if (denominator == 0)
            return true;
        denominator = 1.0f / denominator;
        float alpha = denominator * ((b.y - c.y) * (checkPoint.x - c.x) + (c.x - b.x) * (checkPoint.y - c.y));
        float beta  = denominator * ((c.y - a.y) * (checkPoint.x - c.x) + (a.x - c.x) * (checkPoint.y - c.y));
        float gamma = 1.0f - alpha - beta;

        return !(alpha < 0 || beta < 0 || gamma < 0);
}


void TriangulatePolygon(Vertex* vertex_array, Vector3 normal, Vector2* vector2_array, bool* removed_vertexes, int* index_array, int number_of_points, int* result_indices)
{
	normal = normal * -1;
	Vector3 summ = normal;
	if(summ.x < 0) { summ.x = -summ.x; }
	if(summ.y < 0) { summ.y = -summ.y; }
	if(summ.z < 0) { summ.z = -summ.z; }
	
	
	int ia = 0;
	int ib = 1;
	float ic = normal.z;
	if(summ.x > summ.y)
	{
		if(summ.x > summ.z)
		{
			ia = 1;
			ib = 2;
			ic = normal.x;
		}
	}
	else if(summ.y > summ.z)
	{
		ia = 2;
		ib = 0;
		ic = normal.y;
	}
	if(ic > 0)
	{
		int tmp = ia;
		ia = ib;
		ib = tmp;
	}
	
	Vector2* points_ptr = vector2_array;
	for(int i = 0; i < number_of_points; i++)
	{
		points_ptr->x = vertex_array[index_array[i]].pos[ia];
		points_ptr->y = vertex_array[index_array[i]].pos[ib];
		points_ptr++;
	}
	
	int number_of_unused_points = number_of_points;
	int next = 0;
	int prev = number_of_points - 1;
	int ear;
	int numFound = 0;
	while(number_of_unused_points >= 3)
	{
		numFound = 0;
		for(ear = next;; prev = ear, ear = next)
		{
			//find next point
			for(next = ear + 1; removed_vertexes[(next >= number_of_points ? next = 0 : next)]; next++);
			if (next < ear)
				if (++numFound == 2)
					break;
			
			
			
			Vector2 a = vector2_array[prev], b = vector2_array[ear], c = vector2_array[next];
			if(checkClockwise(a, b, c) == 0) 	{ continue; }
			
			int j = 0;
			for(; j < number_of_points; j++)
			{
				if(j == ear || j == prev || j == next)
				{
					continue;
				}
				if(inTrinagle(a, b, c, vector2_array[j]))
				{
					break;
				}
			}
			if(j != number_of_points) { continue; 	}
			break;
		}
		
		
		*(result_indices++) = index_array[ prev];
		*(result_indices++) = index_array[ ear ];
		*(result_indices++) = index_array[ next];
		removed_vertexes[ear] = 1;
		number_of_unused_points--;
		
	}
	return;
	
	int i = 0;
	for(; i < number_of_points; i++) {
		if(removed_vertexes[i] == 0) {
			//*(result_indices++) = index_array[i++];
			break;
		}
	}
	for(; i < number_of_points; i++) {
		if(removed_vertexes[i] == 0) {
			//*(result_indices++) = index_array[i++];
			break;
		}
	}
	for(; i < number_of_points; i++) {
		if(removed_vertexes[i] == 0) {
			//*(result_indices++) = index_array[i++];
			break;
		}
	}
}


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