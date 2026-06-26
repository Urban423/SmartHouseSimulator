#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include "Quaternion.h"
#include "DirtyValue.h"
#include "TimeSystem.h"
#include "Color.h"
#include "Mesh.h"
#include <vector>
#include <array>
#include <cstring>
#include <string>
#include <functional>

class Transform;

class Object
{
public:
	Object();
	Object(int id, Transform &transform) : id(id), transform(transform) {};
	Object(const Object &other) : id(other.id), transform(other.transform) {};

	inline bool operator==(const int value) { return id == value; }
	inline bool operator==(const Object &obj) { return id == obj.id; }
	inline bool operator!=(const Object &obj) { return id != obj.id; }
	inline int getID() { return id; }
	Object &operator=(const Object &other)
	{
		std::memcpy(this, &other, sizeof(Object));
		return *this;
	}

	template <class T>
	T &AddComponent(int group = 0);
	template <class T>
	T &GetComponent();
	template <class T>
	bool HasComponent();
	void setParent(Object parent);
	void setParent(int parentID);
	Object getParent();

public:
	Transform &transform;

private:
	int id;

private:
	friend class ECS;
	friend class ComponentManager;
};

struct Component
{
	Object object;
	bool enabled;
};

struct Camera : public Component
{
	int renderLayout = 0;
	Color color = Color(0, 0, 0, 1);
	float focalLength = 0.004;
	int frameBufferIndex = -1;
	char perpective = false;
};

struct Transform
{
	Vector3 position;
	Quaternion rotation;
	Vector3 scale = Vector3(1, 1, 1);
};

struct Material
{
	Material() : shader_indexes(0), texture_index(0), texture_index1(0), color(0xFF) {};
	Material(short shader_indexes, int texture_index, int texture_index1, Color color) : shader_indexes(shader_indexes), texture_index(texture_index), texture_index1(texture_index1), color(color) {};
	short shader_indexes = 0;
	int texture_index = 0;
	int texture_index1 = 0;
	Color color = Color(1, 1, 1, 1);
};

struct RenderView : public Component
{
	char layout = 0;
	std::array<short, 4> materals;
	short mesh_index = 0;
};

struct ScreenBlock : public Component
{
};

class TextView : public Component
{
public:
	void buildMesh()
	{
		mesh.rebuildTextMesh(text);
	}
	inline int getId()
	{
		return mesh.id;
	}

	int layout = 0;
	std::string text;
	Color color = Color(1, 1, 1, 1);

private:
	Mesh mesh;
	bool dirty = true;
};

struct Button : public Component
{
	std::function<void()> onMouseDown;
	std::function<void()> onMouseUp;
	bool pressed = false;
};
