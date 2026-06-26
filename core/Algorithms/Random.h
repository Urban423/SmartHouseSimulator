#pragma once
#include <random>

inline void setSeed(unsigned int value) { srand(value); }

inline unsigned int random() { return rand(); };

inline float noise(float min, float max) { 
    float r = random();
    return min + (r / (float)RAND_MAX) * (max - min);
}

template<class T = int>
void generateSortedRandomArray(typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end, T minVal, T maxVal) {
	int n = std::distance(begin, end);
	T step = (maxVal - minVal) / n;
	
	T value = minVal;
	for (auto it = begin; it != end; ++it) {
        value += 0.1f + noise(0, step);
        *it = value;
    }
}


template<class T = int>
void generateRandomArray(typename std::vector<T>::iterator begin, typename std::vector<T>::iterator end, T minVal, T maxVal) {
	for (auto it = begin; it != end; ++it) {
        *it = noise(minVal, maxVal);
    }
}

inline void generateRandomArrayVector2(typename std::vector<Vector2>::iterator begin, typename std::vector<Vector2>::iterator end, Vector2 minVal, Vector2 maxVal) {
	for (auto it = begin; it != end; ++it) {
        *it = {noise(minVal.x, minVal.y), noise(maxVal.x, maxVal.y)};
    }
}