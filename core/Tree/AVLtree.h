#pragma once
#include "Tree.h"

template<typename T>   
class AVLNode: public INode<T> {
public:
    AVLNode() : INode<T>(T{}), parentIndex(UNDENTIFID_NODE), height(1) {}
    AVLNode(const T& v, size_t parentIndex): INode<T>(v), parentIndex(parentIndex), height(1) {}
public:
    void setChild(size_t index, size_t childId);
    void replaceChild(size_t oldChildId, size_t newChildId);
    void updateMetadate();
    size_t getChildId(size_t index) const;
    size_t childrenCount() const;
public:
    int childrenIndexes[2] = {UNDENTIFID_NODE, UNDENTIFID_NODE};
    int parentIndex = UNDENTIFID_NODE;
    int height;
};

template<class T>
class AVLtree: public ITree<T> {
public:
    AVLtree(): ITree<T>() {}
    ~AVLtree() { this->clear(); }

    void reserve(size_t capacity);
    void remove(Cursor<T> cursor) override;
    void clear() override;
    bool empty() const override;
    size_t size() const override;
    ITree<T>* clone() const override;
    Cursor<T> root() const override;
protected:
    void updateHeightsUpwards(size_t startNodeId);
    size_t getNodeId(const INode<T>* node) const override;
    INode<T>* getNode(const size_t nodeId) override;
    void setRoot(INode<T>* newRoot) override;
    size_t chooseBranch(const Cursor<T>& node, const T& value) const;
    INode<T>* createNode(const T& value, size_t parentId) override;
    void rebalance(const Cursor<T>& node) override;
public:
    std::vector<AVLNode<T>> nodes;
    int rootIndex = UNDENTIFID_NODE;
};

#include "AVLtree.hpp"