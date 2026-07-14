#pragma once
#include "Span.h"
#include "stdlibInclude.h"

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
    SparseSet() = default;
    SparseSet(const SparseSet&) = delete;
    SparseSet& operator=(const SparseSet&) = delete;

    SparseSet(SparseSet&& other) noexcept  {
        data = other.data;
        dataSize = other.dataSize;
        capacity = other.capacity;
        count =  other.count;
        sizeOfComponent = other.sizeOfComponent;

        sparse = std::move(other.sparse);
        denseToEntity = std::move(other.denseToEntity);

        destroy = other.destroy;
        move = other.move;

        other.data = nullptr;
        other.dataSize = 0;
        other.capacity = 0;
        other.count = 0;
        other.sizeOfComponent = 0;
    }

    SparseSet& operator=(SparseSet&& other) noexcept {
        if (this == &other) return *this;

        clear();
        ::operator delete(data);

        data = other.data;
        dataSize = other.dataSize;
        capacity = other.capacity;
        count =  other.count;
        sizeOfComponent = other.sizeOfComponent;

        sparse = std::move(other.sparse);
        denseToEntity = std::move(other.denseToEntity);

        destroy = other.destroy;
        move = other.move;

        other.data = nullptr;
        other.dataSize = 0;
        other.capacity = 0;
        other.count = 0;
        other.sizeOfComponent = 0;

        return *this;
    }

    ~SparseSet() {
        clear();
        ::operator delete(data);
    }
public:
    template<class T>
    void init() { 
        sizeOfComponent = sizeof(T);

        destroy = [](void* p) { static_cast<T*>(p)->~T(); };
        move = [](void* dst, void* src) {
            new (dst) T(std::move(*static_cast<T*>(src)));
            static_cast<T*>(src)->~T();
        };
    }
    bool initialized() const { return sizeOfComponent != 0; }

    template<class T>
    Span<T> getData() {
        return Span<T>( reinterpret_cast<T*>(data), count );
    }

    void clear() {
        if (!data || !destroy) return;
        for (size_t i = 0; i < count; i++)
            destroy(data + i * sizeOfComponent);

        count = 0;
        dataSize = 0;
        sparse.clear();
        denseToEntity.clear();
    }

    bool hasByID(int id) const { return id < sparse.size() && sparse[id] != -1; }

    template<class T>
    T& getByID(int id) {  return *reinterpret_cast<T*>(&data[sparse[id] * sizeOfComponent]); }

    template<class T>
    T& add(int id) {
        void* ptr = addRaw(id);
        return *new (ptr) T{};
    }

    void* addRaw(int id) {
        if (id >= sparse.size()) sparse.resize(id + 1, -1);

        size_t offset = dataSize;
        if (offset + sizeOfComponent > capacity)
            reserve(std::max<size_t>(capacity * 2, offset + sizeOfComponent));

        sparse[id] = denseToEntity.size();
        denseToEntity.push_back(id);
        ++count;
        dataSize = offset + sizeOfComponent;
        return data + offset;
    }

    void reserve(size_t newCapacity) {
        if (newCapacity <= capacity) return;

        char* newData = static_cast<char*>( ::operator new(newCapacity) );
        for (int i = 0; i < count; i++) {
            move(newData + i * sizeOfComponent, data + i * sizeOfComponent);
        }

        ::operator delete(data);
        data = newData;
        capacity = newCapacity;
    }

    template<class T>
    int getEntity(const T& component) const {
        const T* begin = reinterpret_cast<const T*>(data);
        int index = &component - begin;
        return denseToEntity[index];
    }

    void remove(int id) {
        if (!hasByID(id)) return;

        int removed = sparse[id];
        int last = count - 1;

        void* dst = data + removed * sizeOfComponent;
        void* src = data + last * sizeOfComponent;

        if (removed != last) {
            destroy(dst);
            move(dst, src);

            int movedEntity = denseToEntity[last];
            denseToEntity[removed] = movedEntity;
            sparse[movedEntity] = removed;
        }
        else {
            destroy(src);
        }

        count--;
        dataSize -= sizeOfComponent;
        denseToEntity.pop_back();
        sparse[id] = -1;
    }

    void serialize(int entityID, std::vector<char>& out) {
        int index = sparse[entityID];
        size_t offset = index * sizeOfComponent;

        append(out, sizeOfComponent);
        append(out, data + offset, sizeOfComponent);
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
    size_t count = 0;
    int sizeOfComponent = 0;
    char* data = nullptr;
    size_t dataSize = 0;
    size_t capacity = 0;

    std::vector<int> sparse;
    std::vector<int> denseToEntity;

    void (*destroy)(void*) = nullptr;
    void (*move)(void*, void*) = nullptr;
};