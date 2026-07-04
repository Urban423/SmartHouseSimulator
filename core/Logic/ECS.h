#pragma once
#include <stack>
#include "ECSCore.h"
#include "Transform.h"
#include "UIManager.h"
#include "HierarchySystem.h"
#define TRANSFORM_ARRAY_SIZE 300

class Object;
class ECS {
public:
	ECS() {
		ecsCore.getComponentID<Transform>();
		ecsCore.getComponentID<RenderView>();
		transformArray.resize(1);
		transformArray[0].reserve(TRANSFORM_ARRAY_SIZE);
	};

	inline static Object createObject() {
		int id = ecs->ecsCore.create();
		int highArrayIndex = id / TRANSFORM_ARRAY_SIZE;
		if (highArrayIndex >= ecs->transformArray.size()) {
			ecs->transformArray.resize(highArrayIndex + 1);
		}
		auto& bucket = ecs->transformArray[highArrayIndex];
		int localIndex = id % TRANSFORM_ARRAY_SIZE;
		if (localIndex >= bucket.size()) {
			bucket.resize(localIndex + 1);
		}
		bucket[localIndex] = Transform();
		ecs->hierarchySystem.addObject(id);
		return Object{ id, bucket[localIndex] };
	}

	inline static void deleteObject(int id) { 
		std::vector<int> children = ecs->hierarchySystem.getChildren(id);
		for (int child : children) {
			deleteObject(child);
		}
		ecs->ecsCore.destroy(id);
	};

	inline static void deleteObject(Object& object) { deleteObject(object.id); };

	inline static Object getObjectByID(int id) { return Object{id, ecs->transformArray[id / TRANSFORM_ARRAY_SIZE][id % TRANSFORM_ARRAY_SIZE]}; }

	template<class T>
	inline static int GetComponentID() { return ecs->ecsCore.getComponentID<T>(); }


	template <class T>
	static T& addComponentToSystem(Object* object, int objectID) {
		T& component = ecs->ecsCore.AddComponent<T>(objectID);
		component.object = *object;
		component.enabled = true;
		return component;
	}

	template <class T>
	inline static T &GetComponent(int objectID) {
		if constexpr (std::is_same_v<T, Transform>) return ecs->transformArray[objectID / TRANSFORM_ARRAY_SIZE] [objectID % TRANSFORM_ARRAY_SIZE];
		return ecs->ecsCore.GetComponent<T>(objectID);
	}

	template <class T>
	inline static bool HasComponent(int objectID) { return ecs->ecsCore.HasComponent<T>(objectID); }

	template <class T>
	inline static Span<T> GetComponents() { return ecs->ecsCore.GetComponents<T>(); }

	inline static HierarchySystem &GetHierarchy() { return ecs->hierarchySystem; }

	
    inline static void SerializeEntity(Object& entity, std::vector<char>& out, long long bitmask = ~0ULL) {
    	int id = entity.id;
		int totalComponents = ecs->ecsCore.countComponents(id);
    	int serializedCount = 0;
		if (bitmask & 0x1) ++serializedCount;

		int componentID = 0;
    	for (int i = 0; i < totalComponents; ++i) {
			componentID = ecs->ecsCore.nextComponentID(id, componentID);
			if (bitmask & (1ULL << componentID)) ++serializedCount; 
		}

		append(out, serializedCount);
    	componentID = 0;
		if(bitmask & 0x1) {
			append(out, componentID);
			append(out, ecs->transformArray[id / TRANSFORM_ARRAY_SIZE][id % TRANSFORM_ARRAY_SIZE]);
		}
		for(int i = 0; i < totalComponents; i++) {
			componentID = ecs->ecsCore.nextComponentID(id, componentID);
			if (!(bitmask & (1ULL << componentID))) continue;

			append(out, componentID);
			ecs->ecsCore.SerializeComponent(id, out, componentID);
		}
	}

	inline static void DeserializeEntity(Object& entity, Span<char>& in, size_t& offset) {
		if(entity.valid() == false) return;
		int id = entity.getID();
        int countComponents;
		int componentID;

        read(in, offset, countComponents);
		for(int i = 0; i < countComponents; i++) {
        	read(in, offset, componentID);
			if(componentID == 0) {
				read(in, offset, ecs->transformArray[id / TRANSFORM_ARRAY_SIZE][id % TRANSFORM_ARRAY_SIZE]);
				continue;
			}
			Component* c = reinterpret_cast<Component*>(ecs->ecsCore.DeserializeComponent(id, in, offset, componentID));
			c->object = entity;
			c->enabled = true;
		}
	}
private:
	static ECS *ecs;

	ECSCore ecsCore;
	std::vector<std::vector<Transform>> transformArray;
	HierarchySystem hierarchySystem;
};

template <class T>
inline T &Object::AddComponent(int group)
{
	return ECS::addComponentToSystem<T>(this, id);
};
template <class T>
inline T &Object::GetComponent() { return ECS::GetComponent<T>(id); };
template <class T>
inline bool Object::HasComponent() { return ECS::HasComponent<T>(id); }
inline void Object::setParent(int parentID) { ECS::GetHierarchy().setParent(id, parentID); }
inline void Object::setParent(Object parent) { ECS::GetHierarchy().setParent(id, parent.id); }
inline Object Object::getParent() { return ECS::getObjectByID(ECS::GetHierarchy().getParent(id)); }

inline Object Object::getChild(int index) { 
	static Transform trash;
	auto& children = ECS::GetHierarchy().getChildren(id);
    if (index < 0 || index >= (int)children.size()) return Object(-1, trash);
    return ECS::getObjectByID(children[index]);
}