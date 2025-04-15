#pragma once
#include <iostream>
#include "../DataStructures/FibonacciHeap.h"
#include <queue>
#include <vector>
#include <limits>
#include <atomic>
#include <utility>

template<typename T>
IGraph<T>::IGraph(unsigned int number_of_vertecies) { 
	this->number_of_vertices = number_of_vertecies;
};


template <class T>
void IGraph<T>::print() {
	for (int i = 0; i < number_of_vertices; i++) {
		for (int j = 0; j < number_of_vertices; j++) {
			T value = getEdge(i, j);
			if(value == inf) { printf("inf\t"); continue;}
			std::cout << getEdge(i, j) << "\t";
		}
		printf("\n");
	}
}

template <class T>
void IGraph<T>::printList() {
	for (int i = 0; i < number_of_vertices; i++) {
		auto it = neighbors(i);
		if(it->hasNext() == false) {
			continue;
		}
		printf("%d -> ", i);
        while (it->hasNext()) {
            auto [neighbor, weight] = it->next();
            if (weight == inf) {
               printf("inf\t"); continue; 
            }
			printf("(%d , weight: %d) ", neighbor, weight);;
        }
		printf("\n");
	}
}

