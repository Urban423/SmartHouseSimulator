#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include <stack>
#include "../DataStructures/krray.h"

#define inf 0x7F'FF 	//0xFF'FF'7F'7F

template<class T>
struct PathData {
	T* dist;
	int* path;
	unsigned int size;
};

template<typename T>
class IGraph 
{
public:
	IGraph(unsigned int number_of_vertecies);
	void print();
	void printList();
	
	T* Dijkstra(int start_vertex);
	T* Dijkstra1(int start_vertex);
	T* BellmanFord(int start_vertex);
	int  FordFulkerson(int source, int sink);
	T** JohnsonsAlgorithm();
	PathData<T> Astar(int start, int end, T (*heuristic)(int, int));
	IGraph<T>* BoruvkaMST(int numThreads);
	krray<int> maxBipartiteMatching();
	void Tarjan();
protected:
	void BoruvkaMSTStep(int* parent, std::atomic<T>* cheapestWeight, std::atomic<int>* cheapestVertex, int start, int end); //?
	int findComponent(int vertex, int* parent); //?
    void unionComponents(int u, int v, int* parent, int* rank); //?
	bool BFS(IGraph<T>* residualGraph, int source, int sink, int* parent); //?????
public:                                                               
	inline unsigned int size() const {return number_of_vertices;}
public:
	virtual			IGraph<T>* clone() 						 		= 0;  
	virtual T	 	getEdge(unsigned int i, unsigned int j) 		= 0; 
	virtual void 	setEdge(unsigned int i, unsigned int j, T w) 	= 0;
	virtual void 	addEdge(unsigned int i, unsigned int j, T w) 	= 0;
	virtual void 	addVertecies(int number_of_vertecies) 			= 0;
	virtual void 	clear()								 			= 0;
public:
	class NeighborIterator {
    public:
        virtual bool hasNext() = 0;
        virtual std::pair<unsigned int, T> next() = 0;
        virtual ~NeighborIterator() = default;
    };
	virtual std::unique_ptr<NeighborIterator> neighbors(unsigned int v) = 0;
protected:
	unsigned int number_of_vertices;
	bool oriented = false;
};



#include "Graph.hpp"
#include "Algorithms/BelmanFord.hpp"
#include "Algorithms/JohnsonsAlgorithm.hpp"
#include "Algorithms/FordFulkerson.hpp"
#include "Algorithms/Dijkstra.hpp"
#include "Algorithms/Astar.hpp"
#include "Algorithms/BoruvkaMST.hpp"
#include "Algorithms/Kuna.hpp"
#include "Algorithms/Tarjan.hpp"
#include "MatrixGraph.h"
#include "ListGraph.h"