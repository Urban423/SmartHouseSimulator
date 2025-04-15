#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Color.h"
#include <vector>
#include <cstring>

class Transform;

class Object {
public:
	Object();
	Object(int id, Transform& transform): 	id(id), transform(transform) 		{};
	Object(const Object& other): id(other.id), transform(other.transform) 		{};
	
	inline bool operator==(const int value) 	{ return id == value;}
	inline bool operator==(const Object& obj) 	{ return id == obj.id;}
	inline bool operator!=(const Object& obj) 	{ return id != obj.id;}
	Object& operator=(const Object& other) {
		std::memcpy(this, &other, sizeof(Object));
		return *this;
	}
	
	template<class T> T& AddComponent(int group = 0);
	template<class T> T& GetComponent();
public:
	Transform& transform;
private:
	int id;
private:
	friend class ECS;
	friend class ComponentManager;
};

struct Component { 
	Object object;
	bool enabled;
};

struct  Camera: public Component {
	Matrix4x4 camView;
	Matrix4x4 projection;
	
	int RenderViewDataIndex;
	float focalLength = 0.004;
	int frameBufferIndex = -1;
	int left = 0;
	int right = 100;
	int top = 0;
	int bottom = 100;
};

struct Transform {
	Vector3 position;
	Quaternion rotation;
	Vector3 scale = Vector3(1, 1, 1);
};

struct RenderView: public Component  {
	int mesh_index = 2;
	int shader_index = 2;
	int texture_index = 0;
	Color color = Color(1, 1, 0, 1);
};