#include "Tree.h"
#include <stack>
#include <queue>


template<typename T>
inline Cursor<T> ITree<T>::add(const T& value) {
    return add(value, root());
}

template<typename T>
inline Cursor<T> ITree<T>::add(const T& value, Cursor<T> parent) {
    INode<T>* newNodePtr = createNode(value, UNDENTIFID_NODE);
    
    if (!parent.isValid()) {
        setRoot(newNodePtr);
        newNodePtr->childrenCount();
        return Cursor<T>(this, getNodeId(newNodePtr));
    }
    Cursor<T> current = parent;
    while(current.isValid()) {
        size_t branch = chooseBranch(current, value);
        if (!current.hasChild(branch)) {
            getNode(current)->setChild(branch, getNodeId(newNodePtr));
            newNodePtr->setParentId(getNodeId(current));
            break;
        }
        current = current.child(branch);
    }
    rebalance(root());
    return Cursor<T>(this, getNodeId(newNodePtr));
}

template<typename T>
inline void ITree<T>::rotateLeft(Cursor<T> cursor) {
    if (!cursor.isValid() || !cursor.hasRight()) return;

    Cursor<T> pivot = cursor.right();
    Cursor<T> pivotLeft = pivot.left();
    size_t pivotLeftIndex = pivotLeft.isValid() ? pivotLeft.index : 0;

    cursor.node->setChild(cursor.childrenCount() - 1, pivotLeft.node);
    pivot.node->setChild(pivotLeftIndex, cursor.node);

    if (cursor.hasParent()) {
        cursor.parent().node->replaceChild(cursor.node, pivot.node);
    } else {
        setRoot(pivot.node);
    }

    cursor.node->updateMetadata(cursor.left()->nodeId, cursor.right()->nodeId);
    pivot.node->updateMetadata(cursor.left()->nodeId, cursor.right()->nodeId);
}


template<typename T>
inline void ITree<T>::rotateRight(Cursor<T> cursor) {
    if (!cursor.isValid() || !cursor.hasLeft()) return;

    Cursor<T> pivot = cursor.left();
    Cursor<T> pivotRight = pivot.right();
    size_t pivotRightIndex = pivotRight.isValid() ?  pivotRight.index : pivot.childrenCount();

    cursor.node->setChild(0, pivotRight.node);
    pivot.node->setChild(pivotRightIndex, cursor.node);

    if (cursor.hasParent()) {
        cursor.parent().node->replaceChild(cursor.node, pivot.node);
    } else {
        setRoot(pivot.node);
    }

    cursor.node->updateMetadata(getNode(cursor.left()->nodeId), getNode(cursor.right()->nodeId));
    pivot.node->updateMetadata(getNode(cursor.left()->nodeId), getNode(cursor.right()->nodeId));
}


template<typename T>
bool ITree<T>::contains(const T& value) const {
    return find(value).isValid();
}

template<typename T>
inline Cursor<T> ITree<T>::find(const T& value) const { 
    return find(value, root());
}

template<typename T>
inline Cursor<T> ITree<T>::find(const T& value, Cursor<T> node) const { 
    return find(value, node, [&](const T& v) { return v == value; });
}

template<typename T>
template<typename Predicate>
inline Cursor<T> ITree<T>::find(const T& value, Cursor<T> node, Predicate predicate) const {
    return find(value, node, predicate, [&](const Cursor<T>& n) { return chooseBranch(n, value); });
}

template<typename T>
template<typename Predicate, typename BranchChooser>
Cursor<T> ITree<T>::find(const T& value, Cursor<T> node, Predicate predicate, BranchChooser branchChooser) const {
    while (node) {
        if (predicate(*node)) return node;
        size_t nextChildIndex = branchChooser(node);
        if (nextChildIndex >= node.childrenCount()) return Cursor<T>();
        node = node.child(nextChildIndex);
    }
    return Cursor<T>();
}


template<typename T>
template<typename Func>
void ITree<T>::dfs(Func func) {
    Cursor<T> root = this->root();
    if (!root.isValid()) return;
    std::vector<Cursor<T>> stack;
    stack.reserve(size());
    stack.push_back(root);

    while (!stack.empty()) {
        Cursor<T> node = stack.back();
        stack.pop_back();

        func(*node);

        for (size_t i = node.childrenCount(); i > 0; --i) {
            stack.push_back(node.child(i - 1));
        }
    }
}

template<typename T>
template<typename Func>
void ITree<T>::bfs(Func func) {
    Cursor<T> root = this->root();
    if (!root.isValid()) return;
    std::vector<Cursor<T>> queue;
    queue.reserve(size());
    size_t head = 0;
    queue.push_back(root);

    while (head < queue.size()) {
        Cursor<T> node = queue[head++];

        func(*node);

        for (size_t i = 0; i < node.childrenCount(); ++i) {
            queue.push_back(node.child(i));
        }
    }
}

template<typename T>
template<typename Func>
void ITree<T>::traverse(Func func, TraversalType type) {
    if (type == TraversalType::BFS) {
        bfs(func);
    } else {
        dfs(func);
    }
}
