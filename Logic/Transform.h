#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "Color.h"
#include <vector>
#include <cstring>
#include <functional>

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
	
	template<class T> T& 	AddComponent(int group = 0);
	template<class T> T& 	GetComponent();
	template<class T> bool 	HasComponent();
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
	Color color				= Color(0, 0, 0, 1);
	float focalLength 		= 0.004;
	int frameBufferIndex 	= -1;
};

struct Transform {
	Vector3 position;
	Quaternion rotation;
	Vector3 scale = Vector3(1, 1, 1);
};

struct RenderView: public Component  {
	std::vector<int> shader_indexes  = { 0 };
	std::vector<int> texture_indexes = { 0 };
	Color color = Color(1, 1, 1, 1);
	int mesh_index = 0;
};

struct Button: public Component  {
	std::function<void()> onMouseDown;
	std::function<void()> onMouseUp;
	bool pressed = false;
};