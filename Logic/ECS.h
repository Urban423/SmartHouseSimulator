#pragma once
#include <stack> 
#include "Transform.h"
#define TRANSFORM_ARRAY_SIZE 300

class Object;

class ComponentManager {
public:
	template<class T>
	inline void init() { sizeOfComponent = sizeof(T); }

	template<class T>
	inline T& getComponent(int objectID) {
		std::pair<int, int> index = objectsInfo[objectID]; 
		T* ptr = reinterpret_cast<T*>(&data[index.first][0] + index.second * sizeof(T));
		return *ptr;
	}
	
	inline void deleteComponent(int objectID) {
		if(objectID >= objectsInfo.size()) { return; }
		std::pair<int, int> index = objectsInfo[objectID]; 
		int group = index.first;
		int removedIndex = index.second;
		int lastIndex = groupCounters[group] - 1;

		if (removedIndex != lastIndex) {
			std::memcpy(
				&data[group][removedIndex * sizeOfComponent],
				&data[group][lastIndex * sizeOfComponent],
				sizeOfComponent
			);
			Component* movedComp = reinterpret_cast<Component*>(&data[group][lastIndex * sizeOfComponent]);
			int movedObjectID = movedComp->object.id;
			objectsInfo[movedObjectID] = {group, removedIndex};
		}

		groupCounters[group]--;
		data[group].resize(groupCounters[group] * sizeOfComponent);
		objectsInfo[objectID] = {-1, -1};
	}

	template<class T>
	inline T& addComponent(const T example, int objectID, int group) {
		if (data.size() <= group) {
			data.resize(group + 1);
			groupCounters.resize(group + 1, 0);
		}
		if (objectsInfo.size() <= objectID) {
			objectsInfo.resize(objectID + 1);
		}

		int index = groupCounters[group]++;
		objectsInfo[objectID] = {group, index};

		size_t offset = index * sizeof(T);
		if (data[group].size() < offset + sizeof(T))
			data[group].resize(offset + sizeof(T));
			
		T* ptr = (T*)&data[group][offset];
		std::memcpy(&data[group][offset], &example, sizeof(T));
		T& cte = *ptr;
		return getComponent<T>(objectID);
	}

	template<class T>
	inline std::pair<T*, int> getPtr(int group) {
		if (group >= data.size()) return { nullptr, 0 };
		return { reinterpret_cast<T*>(&data[group][0]), groupCounters[group] };
	}

private:
	std::vector<std::pair<int, int>> 	objectsInfo;
	std::vector<std::vector<char>> 		data;
	std::vector<int>					groupCounters;
	int sizeOfComponent = 0;
};


class ECS {
public:
	ECS() { getComponentID<Transform>(); getComponentID<RenderView>();  transformArray.resize(1); transformArray[0].reserve(TRANSFORM_ARRAY_SIZE); };
	
	inline static Object createObject() {
		if(ecs->usedID.empty()) {
			if(ecs->transformArray.back().size() == TRANSFORM_ARRAY_SIZE) {
				ecs->transformArray.push_back(std::vector<Transform>());
				ecs->transformArray.back().reserve(TRANSFORM_ARRAY_SIZE);
			}
			
			int highArrayIndex = ecs->objectsCounter / TRANSFORM_ARRAY_SIZE;
			ecs->transformArray[highArrayIndex].push_back(Transform());
			Object newObj {ecs->objectsCounter, ecs->transformArray[highArrayIndex][ecs->objectsCounter % TRANSFORM_ARRAY_SIZE]};
			ecs->objectsCounter++;
			return newObj;
		}
		
		int id = ecs->usedID.top();
		ecs->usedID.pop();
		return Object{id, ecs->transformArray[id / TRANSFORM_ARRAY_SIZE][id % TRANSFORM_ARRAY_SIZE]};
	}
	
	inline static void deleteObject(Object& object) {
		ecs->usedID.push(object.id);
		for(int i = 0; i < ecs->componentManagers.size(); i++) {
			ecs->componentManagers[i].deleteComponent(object.id);
		}
	};
	
	template<class T>
	inline static T& addComponentToSystem(T& example, int objectID, int group) {
		int componentID = ecs->getComponentID<T>();
		if(ecs->componentManagers.size() <= componentID) { ecs->componentManagers.resize(componentID + 1); }
		ecs->componentManagers[componentID].init<T>();
		return ecs->componentManagers[componentID].addComponent<T>(example, objectID, group);
	}
	
	template<class T>
	inline static T& GetComponent(int id) {
		int componentID =  ecs->getComponentID<T>();
		//if(ecs->componentManagers.size() <= componentID) { return {nullptr, 0}; }
		
		return ecs->componentManagers[componentID].getComponent<T>(id);
	}
	
	template<class T>
	inline static std::pair<T*, int> GetComponents(int group = 0) {
		int componentID =  ecs->getComponentID<T>();
		if(ecs->componentManagers.size() <= componentID) { return {nullptr, 0}; }
		return ecs->componentManagers[componentID].getPtr<T>(group);
	}
private:
	static ECS* ecs;
	
	int globlID = 0;
	template<class T>
	inline int getComponentID() {
		static int id = globlID++;
		return id;
	}
	
	int 								objectsCounter = 0;
	std::stack<int> 					usedID;
	std::vector<std::vector<Transform>> transformArray;
	std::vector<ComponentManager> 		componentManagers;
};







template<class T> inline T& Object::AddComponent(int group) { T t{*this, true};  return ECS::addComponentToSystem<T>(t, id, group); };
template<class T> inline T& Object::GetComponent() 			{   return ECS::GetComponent<T>(id); };