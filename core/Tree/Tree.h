#pragma once
#include <cstddef>

#define UNDENTIFID_NODE nullptr
#define REPLACEMENT_ERROR -1

enum class TraversalType {
    BFS,
    DFS
};

template<typename T>
class INode {
public:
    INode(T value): value(value), parent(UNDENTIFID_NODE) {}
    INode(T value, INode<T>* parent): value(value), parent(parent) {}
    ~INode() = default;
public:
    virtual void setChild(size_t index, INode<T>* child) = 0;
    virtual void updateMetadate() = 0;
    virtual INode<T>* getChild(size_t index) const = 0;
    virtual size_t getChildId(INode<T>* child) = 0;
    virtual size_t childrenCapacity() const = 0;
public:
    inline size_t replaceChild(INode<T>* oldChild, INode<T>* newChild) { 
        size_t oldChildId = getChildId(oldChild);
        if(oldChildId == REPLACEMENT_ERROR) return REPLACEMENT_ERROR;
        setChild(oldChildId, newChild);
        newChild->setParent(this);
        return 0;
    }
    inline INode<T>* getParent() const { return parent; };
    inline void   setParent(INode<T>* parent) { this->parent = parent; }
    inline T& getValue() { return value; }
protected:
    T value;
    INode<T>* parent;
};


template<typename T>
class ITree;


template<typename T>
class Cursor {
public:
    Cursor(): node(UNDENTIFID_NODE) {}
    Cursor(ITree<T>* tree): node(tree->root()) {}
    ~Cursor() = default;
public:
    bool isValid() const;
    bool hasParent() const;
    bool hasChild(size_t index) const;
    bool hasLeft() const;
    bool hasRight() const;
    bool isRoot() const;
    size_t childrenCapacity() const;
    
    Cursor<T> child(size_t index) const;
    Cursor<T> root() const;
    Cursor<T> parent() const;
    Cursor<T> left()  const;
    Cursor<T> right() const;
public:
    T& operator*() const;
    Cursor<T> operator[](size_t i) const;
protected:
    INode<T>* node;
};



template<typename T>
class ITree
{
public:
    virtual void clear() = 0;
    virtual bool empty() const = 0;
    virtual size_t size() const = 0;
    virtual Cursor<T> root() const = 0;
    virtual ITree<T>* clone() const = 0;
    virtual void reserve(size_t capacity) = 0;
    virtual void remove(Cursor<T> cursor) = 0;
    
    Cursor<T> add(const T& value);
    Cursor<T> add(const T& value, Cursor<T> parent);

    bool contains(const T& value) const;
    Cursor<T> find(const T& value) const;
    Cursor<T> find(const T& value, Cursor<T> node) const;
    template<typename Predicate>
    Cursor<T> find(const T& value, Cursor<T> node, Predicate predicate) const;
    template<typename Predicate, typename BranchChooser>
    Cursor<T> find(const T& value, Cursor<T> node, Predicate predicate, BranchChooser branchChooser) const;

    
    template<typename Func>
    void dfs(Func func);
    template<typename Func>
    void bfs(Func func);
    template<typename Func>
    void traverse(Func func, TraversalType type = TraversalType::BFS);

    void rotateLeft(Cursor<T> cursor);
    void rotateRight(Cursor<T> cursor);
protected:
    virtual void setRoot(INode<T>* newRoot) = 0;
    virtual INode<T>* createNode(const T& value, size_t parentId) = 0;
    virtual size_t chooseBranch(const Cursor<T>& node, const T& value) const = 0;
    virtual void rebalance(const Cursor<T>& node) = 0;
};

#include "ITree.hpp"
#include "Cursor.hpp"
#include "AVLtree.h"