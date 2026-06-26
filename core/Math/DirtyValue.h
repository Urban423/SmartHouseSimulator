#pragma once

extern unsigned int dirtyFlags;

template <class T, int S>
class DirtyValue
{
public:
    DirtyValue();
    DirtyValue(const T& value);
    DirtyValue &operator=(T newValue);
    operator T() const;
    static bool isDirty(int bit = S);
    static void clear(int bit = S);

private:
    T value;
};

#include "DirtyValue.hpp"