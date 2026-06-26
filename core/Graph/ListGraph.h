#pragma once

template<typename T>
class ListGraph;

template<typename T>
class ListNeighborIterator {
public:
	ListNeighborIterator(ListGraph<T>* graph, unsigned int vertex);
	bool hasNext();
	std::pair<unsigned int, T> next();
private:
	ListGraph<T>* graph;
	unsigned int vertex;
	unsigned int current;
};

template<typename T>
class ListGraph: public IGraph<T, ListNeighborIterator<T>>
{
public:
	ListGraph(bool oriented = false);
	ListGraph(unsigned int number_of_vertices, bool oriented = false);
	~ListGraph();
	
	
	IGraph<T, ListNeighborIterator<T>>* clone();
	T getEdge(unsigned int i, unsigned int j);
	void addEdge(unsigned int i, unsigned int j, T w);
	void setEdge(unsigned int i, unsigned int j, T w);
	void addVertecies(int new_number_of_vertecies);
	void clear();
    ListNeighborIterator<T> neighbors(unsigned int v);
public:
	std::vector<std::vector<std::pair<unsigned int, T>>> data;
};

#include "ListGraph.hpp"