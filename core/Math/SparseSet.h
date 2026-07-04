#pragma once
#include "Span.h"
#include <stack>
#include <vector>
#include <cstdio>
#include <cstring>

template<typename T>
inline void append(std::vector<char>& out, const T& data) {
    size_t old = out.size();
    out.resize(old + sizeof(T));
    memcpy(out.data() + old, &data, sizeof(T));
}

inline void append(std::vector<char>& out, const char* data, const int size) {
    size_t old = out.size();
    out.resize(old + size);
    memcpy(out.data() + old, data, size);
}

template<typename T>
inline void read(const Span<char>& in, size_t& offset, T& data) {
    memcpy(&data, in.begin() + offset, sizeof(T));
    offset += sizeof(T);
}

inline void read(const Span<char>& in, size_t& offset, char* data, size_t size) {
    memcpy(data, in.begin() + offset, size);
    offset += size;
}

class SparseSet {
public:
    template<class T>
    void init() { sizeOfComponent = sizeof(T); }
    bool initialized() const { return sizeOfComponent != 0; }

    template<class T>
    Span<T> getData() {
        return Span<T>(reinterpret_cast<T*>(data.data()), data.size() / sizeof(T));
    }

    void clear() {
        data.clear();
        sparse.clear();
        denseToEntity.clear();
    }

    bool hasByID(int id) const { return id < sparse.size() && sparse[id] != -1; }

    template<class T>
    T& getByID(int id) { 
        return *reinterpret_cast<T*>(&data[sparse[id] * sizeOfComponent]);
    }

    template<class T>
    T& add(int id) {
        void* ptr = addRaw(id);
        return *new (ptr) T{};
    }

    void* addRaw(int id) {
        if (id >= sparse.size()) sparse.resize(id + 1, -1);

        sparse[id] = denseToEntity.size();
        denseToEntity.push_back(id);

        size_t offset = data.size();
        data.resize(offset + sizeOfComponent);

        return data.data() + offset;
    }

    template<class T>
    int getEntity(const T& component) const {
        const T* begin = reinterpret_cast<const T*>(data.data());
        int index = &component - begin;
        return denseToEntity[index];
    }

    void remove(int id) {
        if (!hasByID(id)) return;

        int removed = sparse[id];
        int count = data.size() / sizeOfComponent;
        int last = count - 1;

        if (removed != last) {
            memcpy(&data[removed * sizeOfComponent], &data[last * sizeOfComponent], sizeOfComponent);

            int movedEntity = denseToEntity[last];
            denseToEntity[removed] = movedEntity;
            sparse[movedEntity] = removed;
        }

        data.resize((count - 1) * sizeOfComponent);
        denseToEntity.pop_back();
        sparse[id] = -1;
    }

    void serialize(int entityID, std::vector<char>& out) {
        int index = sparse[entityID];
        size_t offset = index * sizeOfComponent;

        append(out, sizeOfComponent);
        append(out, data.data() + offset, sizeOfComponent);
    }

    char* deserialize(int entityID, const Span<char>& in, size_t& offset) {
        int componentSize;
        read(in, offset, componentSize);
        if(componentSize != sizeOfComponent) return nullptr;

        void* dst;
        if (hasByID(entityID)) {
            dst = &data[sparse[entityID] * sizeOfComponent];
        } else {
            dst = addRaw(entityID);
        }

        read(in, offset, static_cast<char*>(dst), componentSize);
        return static_cast<char*>(dst);;
    }
protected:
    int sizeOfComponent = 0;
    std::vector<char> data;
    std::vector<int> sparse;
    std::vector<int> denseToEntity;
};