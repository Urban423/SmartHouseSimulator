#include "DirtyValue.h"

template <class T, int S>
inline DirtyValue<T, S>::DirtyValue(const T &value) : value(value) { dirtyFlags |= (1u << S); }

template <class T, int S>
inline DirtyValue<T, S>::DirtyValue() { dirtyFlags |= (1u << S); }

template <class T, int S>
inline DirtyValue<T, S> &DirtyValue<T, S>::operator=(T newValue)
{
    value = newValue;
    dirtyFlags |= (1u << S);
    return *this;
}

template <class T, int S>
inline DirtyValue<T, S>::operator T() const { return value; }
template <class T, int S>
inline bool DirtyValue<T, S>::isDirty(int bit) { return (dirtyFlags & (1u << bit)) != 0; }
template <class T, int S>
inline void DirtyValue<T, S>::clear(int bit) { dirtyFlags &= ~(1u << bit); }
