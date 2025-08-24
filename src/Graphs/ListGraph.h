#pragma once
#include "../DataStructures/krray.h"

template<typename T>
class ListGraph: public IGraph<T>
{
public:
	ListGraph(bool oriented = false);
	ListGraph(unsigned int number_of_vertices, bool oriented = false);
	~ListGraph();
	
	
	IGraph<T>* clone();
	T getEdge(unsigned int i, unsigned int j);
	void addEdge(unsigned int i, unsigned int j, T w);
	void setEdge(unsigned int i, unsigned int j, T w);
	void addVertecies(int new_number_of_vertecies);
	void clear();
public:
	class ListNeighborIterator : public IGraph<T>::NeighborIterator {
    public:
        ListNeighborIterator(ListGraph* graph, unsigned int vertex);
        bool hasNext();
        std::pair<unsigned int, T> next();
    private:
        ListGraph* graph;
        unsigned int vertex;
        unsigned int current;
    };
	
	typename std::unique_ptr<typename IGraph<T>::NeighborIterator> neighbors(unsigned int v);
public:
	krray<krray<std::pair<unsigned int, T>>> data;
};

#include "ListGraph.hpp"