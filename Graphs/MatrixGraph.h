#pragma once
#include "../DataStructures/krray.h"

template<typename T>
class MatrixGraph: public IGraph<T>
{
public:
	MatrixGraph(bool oriented = false);
	MatrixGraph(unsigned int number_of_vertices, bool oriented = false);
	IGraph<T>* clone();
	T getEdge(unsigned int i, unsigned int j);
	void addEdge(unsigned int i, unsigned int j, T w);
	void setEdge(unsigned int i, unsigned int j, T w);
	void addVertecies(int new_number_of_vertecies);
	void clear();
public:
	class MatrixNeighborIterator : public IGraph<T>::NeighborIterator {
    public:
        MatrixNeighborIterator(MatrixGraph* graph, unsigned int vertex);
        bool hasNext();
        std::pair<unsigned int, T> next();
    private:
        MatrixGraph* graph;
        unsigned int vertex;
        unsigned int current;
    };
	
	std::unique_ptr<typename IGraph<T>::NeighborIterator> neighbors(unsigned int v);
private:
	krray<T> data;
};


#include "MatrixGraph.hpp"