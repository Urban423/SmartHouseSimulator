#pragma once

template<typename T>
class MatrixGraph;

template<typename T>
class MatrixNeighborIterator  {
public:
	MatrixNeighborIterator(MatrixGraph<T>* graph, unsigned int vertex);
	bool hasNext();
	std::pair<unsigned int, T> next();
private:
	MatrixGraph<T>* graph;
	unsigned int vertex;
	unsigned int current;
};

template<typename T>
class MatrixGraph: public IGraph<T, MatrixNeighborIterator<T>>
{
public:
	MatrixGraph(bool oriented = false);
	MatrixGraph(unsigned int number_of_vertices, bool oriented = false);
	IGraph<T, MatrixNeighborIterator<T>>* clone();
	T getEdge(unsigned int i, unsigned int j);
	void addEdge(unsigned int i, unsigned int j, T w);
	void setEdge(unsigned int i, unsigned int j, T w);
	void addVertecies(int new_number_of_vertecies);
	void clear();
	MatrixNeighborIterator<T> neighbors(unsigned int v);
private:
	std::vector<T> data;
	friend class MatrixNeighborIterator<T>;
};


#include "MatrixGraph.hpp"