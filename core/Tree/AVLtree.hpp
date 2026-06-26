#include "AVLtree.h"

template<typename T>
void AVLNode<T>::setChild(size_t index, size_t childId) {
    childrenIndexes[index] = childId;
}

template<typename T>
void AVLNode<T>::replaceChild(size_t oldChildId, size_t newChildId) {
    if (childrenIndexes[0] == oldChildId) {
        childrenIndexes[0] = newChildId;
    } else if (childrenIndexes[1] == oldChildId) {
        childrenIndexes[1] = newChildId;
    }
}

template<typename T>
size_t AVLNode<T>::getChildId(size_t index) const {
    return childrenIndexes[index];
}

template<typename T>
size_t AVLNode<T>::childrenCount() const {
    return (childrenIndexes[0] != UNDENTIFID_NODE) + (childrenIndexes[1] != UNDENTIFID_NODE);
}

template<typename T>
void AVLNode<T>::updateMetadate() {
    int leftHeight  = (childrenIndexes[0] != UNDENTIFID_NODE) ? getChild(0)->height : 0;
    int rightHeight = (childrenIndexes[1] != UNDENTIFID_NODE) ? getChild(1)->height : 0;
    height = 1 + std::max(leftHeight, rightHeight);
}






template<class T>
void AVLtree<T>::rebalance(const Cursor<T>& node) {
    
}

template<class T>
inline INode<T>* AVLtree<T>::createNode(const T& value, size_t parentId) {
    AVLNode<T> newNode(value, parentId);
    nodes.push_back(newNode);
    return &nodes.back();
}

template<class T>
inline void AVLtree<T>::reserve(size_t capacity) { 
    nodes.reserve(capacity);
}

template<class T>
inline void AVLtree<T>::remove(Cursor<T> cursor) {

}

template<class T>
inline void AVLtree<T>::clear() {
    nodes.clear();
}

template<class T>
inline bool AVLtree<T>::empty() const {
    return size() == 0;
}

template<class T>
inline size_t AVLtree<T>::size() const {
    return nodes.size();
}

template<class T>
inline ITree<T>* AVLtree<T>::clone() const {
    AVLtree<T>* newTree = new AVLtree<T>();
    newTree->nodes = nodes;
    return newTree;
}

template<class T>
inline Cursor<T> AVLtree<T>::root() const {
    if(rootIndex == UNDENTIFID_NODE) { return Cursor<T>(); }
    return Cursor<T>(const_cast<AVLtree<T>*>(this), getNodeId(&nodes[rootIndex]));
}

template<class T>
inline void AVLtree<T>::setRoot(INode<T>* newRoot) {
    rootIndex = getNodeId(newRoot);
}

template<class T>
inline size_t AVLtree<T>::chooseBranch(const Cursor<T>& node, const T& value) const {
    return (*node < value) ? 0 : 1;
}

template<class T>
inline size_t AVLtree<T>::getNodeId(const INode<T>* node) const{
    return static_cast<size_t>(static_cast<const AVLNode<T>*>(node) - &nodes[0]);
}

template<class T>
inline INode<T>* AVLtree<T>::getNode(const size_t nodeId) {
    return &nodes[nodeId];
}


template<typename T>
void AVLtree<T>::updateHeightsUpwards(size_t nodeId) {
    while (nodeId != UNDENTIFID_NODE) {
        AVLNode<T>* node = static_cast<AVLNode<T>*>(getNode(nodeId));

        int leftHeight = (node->childrenIndexes[0] != UNDENTIFID_NODE)
                         ? static_cast<AVLNode<T>*>(getNode(node->childrenIndexes[0]))->height
                         : 0;

        int rightHeight = (node->childrenIndexes[1] != UNDENTIFID_NODE)
                          ? static_cast<AVLNode<T>*>(getNode(node->childrenIndexes[1]))->height
                          : 0;

        int newHeight = 1 + std::max(leftHeight, rightHeight);

        if (node->height == newHeight)
            break;

        node->height = newHeight;

        rebalance(nodeId);

        nodeId = node->parentIndex;
    }
}
