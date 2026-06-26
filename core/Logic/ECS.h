#pragma once
#include <stack>
#include "Transform.h"
#include "UIManager.h"
#include "HierarchySystem.h"
#define TRANSFORM_ARRAY_SIZE 300

class Object;

class ComponentManager
{
public:
	template <class T>
	inline void init() { sizeOfComponent = sizeof(T); }

	template <class T>
	inline T &getComponent(int objectID)
	{
		int index = objectsInfo[objectID];
		T *ptr = reinterpret_cast<T *>(&data[0] + index * sizeof(T));
		return *ptr;
	}

	inline void deleteComponent(int objectID)
	{
		if (objectID >= objectsInfo.size())
			return;
		int index = objectsInfo[objectID];
		if (index == -1)
			return;

		int removedIndex = index;
		int lastIndex = counter - 1;
		if (removedIndex != lastIndex)
		{
			std::memcpy(
				&data[removedIndex * sizeOfComponent],
				&data[lastIndex * sizeOfComponent],
				sizeOfComponent);
			Component *movedComp = reinterpret_cast<Component *>(&data[lastIndex * sizeOfComponent]);
			int movedObjectID = movedComp->object.id;
			objectsInfo[movedObjectID] = removedIndex;
		}

		counter--;
		data.resize(counter * sizeOfComponent);
		objectsInfo[objectID] = -1;
	}

	template <class T>
	inline T &addComponent(Object *object, int objectID)
	{
		if (objectsInfo.size() <= objectID)
			objectsInfo.resize(objectID + 1, -1);

		int index = counter++;
		objectsInfo[objectID] = index;

		size_t offset = index * sizeof(T);
		if (data.size() < offset + sizeof(T))
			data.resize(offset + sizeof(T));

		T *ptr = new (&data[offset]) T();
		ptr->object = *object;
		ptr->enabled = true;
		return *ptr;
	}

	template <class T>
	inline T *getPtr() { return reinterpret_cast<T *>(&data[0]); }
	inline int size() { return counter; }

	template <class T>
	inline bool hasComponent(int objectID)
	{
		if (objectID >= objectsInfo.size())
			return false;
		int index = objectsInfo[objectID];
		if (index == -1)
			return false;
		return true;
	}

private:
	std::vector<int> objectsInfo;
	std::vector<char> data;
	int counter = 0;
	int sizeOfComponent = 0;
};

class ECS
{
public:
	ECS()
	{
		getComponentID<Transform>();
		getComponentID<RenderView>();
		transformArray.resize(1);
		transformArray[0].reserve(TRANSFORM_ARRAY_SIZE);
	};

	inline static Object createObject()
	{
		if (ecs->usedID.empty())
		{
			if (ecs->transformArray.back().size() == TRANSFORM_ARRAY_SIZE)
			{
				ecs->transformArray.push_back(std::vector<Transform>());
				ecs->transformArray.back().reserve(TRANSFORM_ARRAY_SIZE);
			}

			int highArrayIndex = ecs->objectsCounter / TRANSFORM_ARRAY_SIZE;
			ecs->transformArray[highArrayIndex].push_back(Transform());
			Object newObj{ecs->objectsCounter, ecs->transformArray[highArrayIndex][ecs->objectsCounter % TRANSFORM_ARRAY_SIZE]};
			ecs->hierarchySystem.addObject(ecs->objectsCounter);
			ecs->objectsCounter++;
			return newObj;
		}

		int id = ecs->usedID.top();
		ecs->usedID.pop();
		ecs->hierarchySystem.addObject(id);
		return Object{id, ecs->transformArray[id / TRANSFORM_ARRAY_SIZE][id % TRANSFORM_ARRAY_SIZE]};
	}
	inline static Object getObjectByID(int id)
	{
		return Object{id, ecs->transformArray[id / TRANSFORM_ARRAY_SIZE][id % TRANSFORM_ARRAY_SIZE]};
	}
	inline static void deleteObject(Object &object)
	{
		ecs->usedID.push(object.id);
		for (int i = 0; i < ecs->componentManagers.size(); i++)
		{
			ecs->componentManagers[i].deleteComponent(object.id);
		}
	};

	template <class T>
	inline static T &addComponentToSystem(Object *object, int objectID)
	{
		int componentID = ecs->getComponentID<T>();
		if (ecs->componentManagers.size() <= componentID)
			ecs->componentManagers.resize(componentID + 1);

		ecs->componentManagers[componentID].init<T>();
		T& newT = ecs->componentManagers[componentID].addComponent<T>(object, objectID);
		// if constexpr (std::is_same_v<T, UIElement>)
		// {
		// 	ecs->uiManager.add();
		// }
		return newT;
	}

	template <class T>
	inline static T &GetComponent(int objectID)
	{
		if constexpr (std::is_same_v<T, Transform>)
		{
			return ecs->transformArray[objectID / TRANSFORM_ARRAY_SIZE]
									  [objectID % TRANSFORM_ARRAY_SIZE];
		}
		// else if constexpr (std::is_same_v<T, RenderView>)
		// {
		// 	return ecs->renderManager.getRenderView(objectID);
		// }
		else
		{
			int componentID = ecs->getComponentID<T>();
			return ecs->componentManagers[componentID].getComponent<T>(objectID);
		}
	}

	template <class T>
	inline static bool HasComponent(int objectID)
	{
		int componentID = ecs->getComponentID<T>();
		if (ecs->componentManagers.size() <= componentID)
			return false;
		return ecs->componentManagers[componentID].hasComponent<T>(objectID);
	}

	template <class T>
	inline static std::pair<T *, int> GetComponents()
	{
		int componentID = ecs->getComponentID<T>();
		if (ecs->componentManagers.size() <= componentID)
			return {nullptr, 0};
		return {ecs->componentManagers[componentID].getPtr<T>(), ecs->componentManagers[componentID].size()};
	}

	inline static HierarchySystem &GetHierarchy() { return ecs->hierarchySystem; }
	// inline static UIManager &GetUIManager() { return ecs->uiManager; }

private:
	static ECS *ecs;

	int globlID = 0;
	template <class T>
	inline int getComponentID()
	{
		static int id = globlID++;
		return id;
	}

	int objectsCounter = 0;
	std::stack<int> usedID;
	std::vector<std::vector<Transform>> transformArray;
	std::vector<ComponentManager> componentManagers;
	HierarchySystem hierarchySystem;
	// UIManager uiManager;
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