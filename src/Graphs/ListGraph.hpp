#include "ListGraph.h"


template<typename T>
ListGraph<T>::ListGraph(bool oriented): IGraph<T>(0) 
{
	this->oriented = oriented;
}


template<typename T>
ListGraph<T>::ListGraph(unsigned int number_of_vertices, bool oriented): IGraph<T>(number_of_vertices)  {
	data.resize(number_of_vertices);
	this->oriented = oriented;
	for(int i = 0; i < number_of_vertices; i++) {
		data[i] = krray<std::pair<unsigned int, T>>();
	}
};


template<typename T>
ListGraph<T>::~ListGraph() {
}

template<typename T>
IGraph<T>* ListGraph<T>::clone() 
{
	ListGraph<T>* copy = new ListGraph<T>(this->number_of_vertices);
	for(int i = 0; i < this->number_of_vertices; i++) {
		copy->data[i] = this->data[i];
	}
	return copy;
}


template<typename T>
inline T ListGraph<T>::getEdge(unsigned int i, unsigned int j) {
	if(i == j) {return T();}
	for (int k = 0; k < data[i].size(); k++) {
		auto& edge =  data[i][k];
		if (edge.first == j) {
			return edge.second;
		}
	}
	return inf;
}

template<typename T>
inline void ListGraph<T>::addEdge(unsigned int i, unsigned int j, T w) {
	for (int k = 0; k < data[i].size(); k++) {
		auto& edge =  data[i][k];
		if (edge.first == j) {
			return;
		}
	}
	data[i].push_back(std::make_pair(j, w));
	if(this->oriented) {
		data[j].push_back(std::make_pair(i, w));
	}
}

template<typename T>
inline void ListGraph<T>::setEdge(unsigned int i, unsigned int j, T w) {
	for (int k = 0; k < data[i].size(); k++) {
		auto& edge =  data[i][k];
		if (edge.first == j) {
			edge.second = w;
			return;
		}
	}
}

template<typename T>
void ListGraph<T>::clear() {
	for (int k = 0; k < data.size(); ++k) {
		data[k].clear();
	}
}



template<typename T>
void ListGraph<T>::addVertecies(int new_number_of_vertecies) 
{
	int old_number_of_vertices = this->number_of_vertices;
	this->number_of_vertices += new_number_of_vertecies;
	data.resize(this->number_of_vertices);
	for (int i = old_number_of_vertices; i < this->number_of_vertices; ++i) 
	{
        data[i] = krray<std::pair<unsigned int, T>>();
    }
}

template<typename T>
inline std::unique_ptr<typename IGraph<T>::NeighborIterator> ListGraph<T>::neighbors(unsigned int v) {
	return std::make_unique<ListNeighborIterator>(this, v);
}








//iterator
template<typename T>
ListGraph<T>::ListNeighborIterator::ListNeighborIterator(ListGraph* graph, unsigned int vertex)
: graph(graph), vertex(vertex), current(0)  {}

template<typename T>
bool ListGraph<T>::ListNeighborIterator::hasNext() {
	while (current < graph->data[vertex].size() && graph->data[vertex][current].second == inf) {
	   ++current;
	}
	return current < graph->data[vertex].size();
}

template<typename T>
std::pair<unsigned int, T> ListGraph<T>::ListNeighborIterator::next() {
	return graph->data[vertex][current++];
};