#pragma once
#include "stdlibInclude.h"
#include "SparseSet.h"
#include <vector>

class ECSCore {
public:
    template <class T>
	inline int getComponentID() {
		static int id = componentID++;
		return id;
	}

    template<class T>
    inline constexpr SparseSet& getPool() {
        int id = getComponentID<T>();
        if (components.size() <= id) components.resize(std::max<size_t>(components.size() * 2, id + 1));
        if (!components[id].initialized()) components[id].init<T>();

        return components[id];
    }

	template <class T>
	inline bool HasComponent(int objectID) { return getPool<T>().hasByID(objectID); }

	template <class T>
	inline T& GetComponent(int objectID) { return getPool<T>().template getByID<T>(objectID); }

	template<class T>
    inline T& AddComponent(int objectID) { return getPool<T>().template add<T>(objectID); }

	template <class T>
	inline Span<T> GetComponents() { return getPool<T>().template getData<T>(); }

    template<class T>
    inline void RemoveComponent(int objectID) { getPool<T>().remove(objectID); }
    
    template<class T>
    int GetEntity(const T& component) { return getPool<T>().getEntity(component); }

    int create() {
        if (!freeIDs.empty()) {
            int id = freeIDs.top();
            freeIDs.pop();
            return id;
        }
        return nextID++;
    }

    void destroy(int objectID) {
        for (auto& pool: components) {
            if (pool.initialized()) pool.remove(objectID);
        }
        freeIDs.push(objectID);
    }

    int countComponents(int entityID) {
        int count = 0;
        for (int i = 0; i < components.size(); ++i) if (components[i].initialized() && components[i].hasByID(entityID)) ++count;
        return count;
    }

    int nextComponentID(int entityID, int componentID) {
        ++componentID;
        while(componentID < components.size()) {
            if (components[componentID].initialized() && components[componentID].hasByID(entityID)) {
                return componentID;
            }
            ++componentID;
        }
        return -1;
    }

    void SerializeComponent(int entityID, std::vector<char>& out, int componentID) {
        if (components[componentID].initialized() && components[componentID].hasByID(entityID)) {
            components[componentID].serialize(entityID, out);
        }
    }

    char* DeserializeComponent(int entityID, Span<char>& in, size_t& offset, int componentID) {
        return components[componentID].deserialize(entityID, in, offset);
    }
private:
    std::vector<SparseSet> components;
    std::stack<int> freeIDs;
    int nextID = 0;
	int componentID = 0;
};