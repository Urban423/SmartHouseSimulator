#include "IOSystem.h"
#include <Triangulation.h>
#include "VertexDictionary.h"
#include <cstdio>

struct VertexWithNormal {
	int index;
	int uv_index;
	Vector3 normal;

	bool operator==(const VertexWithNormal second) {
		if(this->uv_index == second.uv_index && 
		  (this->normal.x == second.normal.x) && 
		  (this->normal.y == second.normal.y) && 
		  (this->normal.z == second.normal.z)) return 1;
		return 0;
	}
};


struct VertexWithoutNormal {
	int index;
	int uv_index;
	int normal_index;

	bool operator==(const VertexWithoutNormal second) {
		if(this->uv_index == second.uv_index && this->normal_index == second.normal_index) return 1;
		return 0;
	}
};

Mesh createMeshWithIndexNormals(
	int* indicies_array, int number_of_indicies,
	Vector3* Vertexes, int  number_of_vertexes,
	Vector2* uv_vertexes,	int* uv_indexes,		int number_of_uv_indexes,
	Vector3* normals,		int* normal_indexes,	int normals_length,
	int* material_indexes, int number_of_materials,	bool byVertex)
{
	//count real number of indecies for trinagles
	bool* removed_vertexes  = new bool[number_of_vertexes];
	int* helping_indicies = new int[number_of_indicies];
	VertexDictionary<VertexWithoutNormal> dictionary(number_of_vertexes);
	int real_number_of_indicies = 0;
	int real_first = 0;
	int first = 0;
	int number_of_points;
	int max_number_of_points_per_polygon = 0;
	
	int number_of_used_materials = number_of_materials > 1? material_indexes[0] : 0;
	int* material_indexes_copy = material_indexes;
	for(int i = 1; i < number_of_materials; i++) { 
		if(material_indexes[i] > number_of_used_materials) { number_of_used_materials = material_indexes[i]; }
	}
	++number_of_used_materials;
	int* materials_counter = new int[number_of_used_materials]; memset(materials_counter, 0, sizeof(int) * number_of_used_materials);
	
	for(int i = 0; i < number_of_indicies; i++) {
		//check the end of the polygon
		if(!(indicies_array[i] & 0x80'00'00'00)) {
			helping_indicies[i] = dictionary.addElement({i, uv_indexes[i], normal_indexes[i]}, indicies_array[i]);
			continue;
		}
		helping_indicies[i] = dictionary.addElement({i, uv_indexes[i], normal_indexes[i]}, -indicies_array[i] - 1);
		
		//get the number of points
		number_of_points = i - first + 1;
		if(number_of_points < 3) {first = i + 1; continue;}
		if(max_number_of_points_per_polygon < number_of_points) { max_number_of_points_per_polygon = number_of_points;}
		real_number_of_indicies += number_of_points * 3 - 6;
		first = i + 1;
		if(number_of_used_materials > 1) { materials_counter[*material_indexes] += number_of_points * 3 - 6; ++material_indexes; }
	}
	
	int* startMaterialPointers;
	if(number_of_used_materials > 1) { 
		startMaterialPointers = new int[number_of_used_materials]; 
		startMaterialPointers[0] = 0;
		for(int i = 1; i < number_of_used_materials; i++) {
			startMaterialPointers[i] = startMaterialPointers[i - 1] + materials_counter[i - 1];
		}
	}
	
	Vertex* result_vertexes = new Vertex[dictionary.getNumberOfElements()];
	Vector2* vector_array = new Vector2[max_number_of_points_per_polygon];
	int* result_indices = new int[real_number_of_indicies];
	first = 0;
	int vertex_index = 0;
	for(int i = 0; i < number_of_indicies; i++) {
		//check the end of the polygon
		if(!(indicies_array[i] & 0x80'00'00'00)) { continue; 	}
		
		//prepare data
		indicies_array[i]++;
		indicies_array[i] *= -1;
		i++;
		
		//get the number of points
		number_of_points = i - first;
		if(number_of_points < 3) {first = i; continue;}
		memset(removed_vertexes, 0, number_of_points);
		
		
		Vector3 helpNormal = byVertex? normals[normal_indexes[indicies_array[first]]] : normals[normal_indexes[first]];
		for(int j = first; j < i; j++) {
			if(j == helping_indicies[j]) {
				if(byVertex) {
					result_vertexes[vertex_index] = {Vertexes[indicies_array[j]], normals[normal_indexes[   indicies_array[j]  ]], uv_vertexes[uv_indexes[j]]};
				}
				else {
					result_vertexes[vertex_index] = {Vertexes[indicies_array[j]], normals[normal_indexes[j]], uv_vertexes[uv_indexes[j]]};
				}
				indicies_array[j] = vertex_index++;
			}
			else {
				indicies_array[j] = indicies_array[helping_indicies[j]];
			}
		}
		
		if(number_of_used_materials > 1) { real_first = startMaterialPointers[*material_indexes_copy]; startMaterialPointers[*material_indexes_copy] += number_of_points * 3 - 6; ++material_indexes_copy; };	
		TriangulatePolygon3D(result_vertexes, helpNormal, vector_array, removed_vertexes, indicies_array + first, number_of_points, result_indices + real_first);
	
		real_first += number_of_points * 3 - 6; 
		first = i;
	}
	if(number_of_used_materials < 2) { materials_counter[0] = real_number_of_indicies;}
	
	delete[] removed_vertexes;
	delete[] helping_indicies;
	delete[] vector_array;
	
	return {result_indices, real_number_of_indicies, result_vertexes, dictionary.getNumberOfElements(), (unsigned int*)materials_counter, (unsigned int)number_of_used_materials};
}





Mesh readObject(Node* geometry) {
	Node* Vertices 		= FBXfile::findChildByName("Vertices", geometry);
	Node* Indecies 		= FBXfile::findChildByName("PolygonVertexIndex", geometry);
	Node* LayerNormal	= FBXfile::findChildByName("LayerElementNormal", geometry);
	Node* Normals 		= FBXfile::findChildByName("Normals", LayerNormal);
	Node* NormalsIndex 	= FBXfile::findChildByName("NormalsIndex", LayerNormal);
	Node* MappInfoType 	= FBXfile::findChildByName("MappingInformationType", LayerNormal);
	Node* RefInfoType	= FBXfile::findChildByName("ReferenceInformationType", LayerNormal);
	Node* LayerUV		= FBXfile::findChildByName("LayerElementUV", geometry);
	Node* UV 			= FBXfile::findChildByName("UV", LayerUV);
	Node* UVIndex 		= FBXfile::findChildByName("UVIndex", LayerUV);
	Node* LayerMaterial	= FBXfile::findChildByName("LayerElementMaterial", geometry);
	Node* Materials		= FBXfile::findChildByName("Materials", LayerMaterial);
	
	// if(1) {
		// Node* toPrint = Materials;
		// for(int i = 0; i < toPrint->children.size(); i++) {
			// //if(!strcmp(toPrint->children[i]->name, "Material")) { continue; } 
			// //printf("%s\n\tproperties:\n", toPrint->children[i]->name); for(int j = 0; j < toPrint->children[i]->props.size(); j++) { printf("\t\t%d\n", toPrint->children[i]->props[j].parametr); }
			// printf("\tchildren:\n");
			// for(int j = 0; j < toPrint->children[i]->children.size(); j++) {
				// printf("\t\t%s\n", toPrint->children[i]->children[j]->name);
			// }
			// printf("\n");
		// }
	// }
	
	//indexes
	int* indicies_array = Indecies->props[0].IntegerArray;
	int  number_of_indicies = Indecies->props[0].ArrayLength;
	
	
	//vertexes
	int  number_of_vertexes = Vertices->props[0].ArrayLength / 3;
	Vector3* Vertexes = new Vector3[number_of_vertexes];
	for(int i = 0, j = 0; i < Vertices->props[0].ArrayLength; i += 3, j++) {
		Vertexes[j] = Vector3(Vertices->props[0].DoubleArray[i], Vertices->props[0].DoubleArray[i + 2], Vertices->props[0].DoubleArray[i + 1]);
	}
	
	//normals
	int normals_length 	= Normals->props[0].ArrayLength / 3;
	int* normal_indexes;
	Vector3* normals = new Vector3[normals_length];
	bool byVertex = true;
	for(int i = 0, j = 0; j < normals_length; i += 3, j++) {
		normals[j] = Vector3(Normals->props[0].DoubleArray[i], Normals->props[0].DoubleArray[i + 2], Normals->props[0].DoubleArray[i + 1]); 
	}
	if(NormalsIndex != nullptr) {
		normal_indexes = NormalsIndex->props[0].IntegerArray;
	}
	else {
		normal_indexes = new int[normals_length];
		for(int i = 0; i < normals_length; i++) {
			normal_indexes[i] = i;
		}
	}
	if(MappInfoType != nullptr) {
		byVertex = MappInfoType->props[0].rawData[2] == 'V';
	}
	
	//uv
	int number_of_uv_indexes = UVIndex->props[0].ArrayLength;
	int* uv_indexes = UVIndex->props[0].IntegerArray;
	int number_of_uv_vertexes = UV->props[0].ArrayLength / 2;
	Vector2* uv_vertexes = new Vector2[number_of_uv_vertexes];
	for(int i = 0, j = 0; i < UV->props[0].ArrayLength; i+=2, j++) {
		uv_vertexes[j] = Vector2(UV->props[0].DoubleArray[i], UV->props[0].DoubleArray[i + 1]);
	}
	
	//material
	int number_of_materials = 0;
	int* material_indexes = nullptr;
	if(Materials != nullptr) {
		number_of_materials= Materials->props[0].ArrayLength;
		material_indexes = Materials->props[0].IntegerArray;
	}
	
	Mesh mesh = createMeshWithIndexNormals(
			indicies_array, number_of_indicies, 
			Vertexes, number_of_vertexes, 
			uv_vertexes, uv_indexes, number_of_uv_indexes, 
			normals, normal_indexes, normals_length, 
			material_indexes, number_of_materials, 
		byVertex
	);
	delete[] Vertexes;
	delete[] normals;
	delete[] uv_vertexes;
	return mesh;
}

std::vector<Mesh> IOSystem::readFBX(const char* filename) {
	CFile f = openCFile(filename);;
	if(f.isEmpty()) { return std::vector<Mesh>(); }
	FBXfile fbxFile(f);
	
	Node* objects 					= FBXfile::findChildByName   ("Objects",  fbxFile.getRoot());
	std::vector<Node*> geometries 	= FBXfile::findChildrenByName("Geometry", objects);
	
	std::vector<Mesh> meshes;
	for(int i = 0; i < geometries.size(); i++) {
		meshes.push_back(readObject(geometries[i]));
		// printf("Mesh %d: %d vertexes, %d indecies\n", i, meshes.back().vertex_size, meshes.back().index_size);
		// for(int i =0 ; i < meshes.back().vertex_size; i++) {
		// 	printf("%f %f %f\t%f %f %f\t%f %f\n", 
		// 		meshes.back().vertex[i].pos.x, meshes.back().vertex[i].pos.y, meshes.back().vertex[i].pos.z, 
		// 		meshes.back().vertex[i].normal.x, meshes.back().vertex[i].normal.y, meshes.back().vertex[i].normal.z,
		// 		meshes.back().vertex[i].uv.x, meshes.back().vertex[i].uv.y);
		// }
	}
	return meshes;
}