#include "Tree.h"

template<typename T>
inline bool Cursor<T>::isValid() const { 
    return node != UNDENTIFID_NODE;
}

template<typename T>
inline bool Cursor<T>::hasParent() const { 
    if (!isValid()) return false;
    return node->getParent() != UNDENTIFID_NODE;
}


template<typename T>
inline bool Cursor<T>::hasChild(size_t index) const {
    if (!isValid()) return false;
    if (index >= node->childrenCapacity()) return false;
    return node->getChild(index) != UNDENTIFID_NODE;
}

template<typename T>
inline bool Cursor<T>::hasLeft() const { 
    return hasChild(0); 
}

template<typename T>
inline bool Cursor<T>::hasRight() const { 
    return hasChild(childrenCapacity() - 1);
}

template<typename T>
inline bool Cursor<T>::isRoot() const { 
    if (!isValid()) return false;
    return node->getParent() == UNDENTIFID_NODE;
}

template<typename T>
inline size_t Cursor<T>::childrenCapacity() const {
    if (!isValid()) return 0;
    return node->childrenCapacity();
}

template<typename T>
inline Cursor<T> Cursor<T>::child(size_t index) const {
    if (!isValid()) return Cursor<T>();
    if (index >= node->childrenCapacity()) return Cursor<T>();
    return Cursor<T>(node->getChild(index));
}


template<typename T>
inline Cursor<T> Cursor<T>::root() const {
    if (!isValid()) return Cursor<T>();
    INode<T>* cur = node;
    while(cur->getParent() != UNDENTIFID_NODE) {
        cur = cur->getParent();
    }
    return Cursor<T>(cur);
}

template<typename T>
inline Cursor<T> Cursor<T>::parent() const {
    if (!isValid()) return Cursor<T>();
    return Cursor<T>(node->getParent());
}

template<typename T>
inline Cursor<T> Cursor<T>::left()  const {
    return hasLeft() ? child(0) : Cursor<T>();    
}

template<typename T>
inline Cursor<T> Cursor<T>::right() const {
    return hasRight() ? child(childrenCapacity() - 1) : Cursor<T>();
}

template<typename T>
inline T& Cursor<T>::operator*() const {
    return node->getValue();
}

template<typename T>
inline Cursor<T> Cursor<T>::operator[](size_t i) const {
    return child(i);
}