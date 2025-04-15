#include "../Graph.h"

template <class T>
T* IGraph<T>::BellmanFord(int start_vertex) {		
	if(number_of_vertices <= start_vertex) {return nullptr;}
	T* dist = new T[number_of_vertices];
	for(unsigned int i = 0; i < number_of_vertices; ++i) { dist[i] = inf; }
	dist[start_vertex] = 0;
	
	for (unsigned int i = 1; i < number_of_vertices; ++i) {
		bool updated = false;
		for (unsigned int u = 0; u < this->number_of_vertices; ++u) {
			auto it = neighbors(u);
			while (it->hasNext()) {
				auto [v, weight] = it->next();
				if (dist[u] != inf && dist[u] + weight < dist[v]) {
					dist[v] = dist[u] + weight;
					updated = true;
				}
			}
		}
		if (!updated) break;
	}
	
	for (unsigned int u = 0; u < this->number_of_vertices; ++u) {
		auto it = neighbors(u);
		while (it->hasNext()) {
			auto [v, weight] = it->next();
			if (dist[u] != inf && dist[u] + weight < dist[v]) {
				delete[] dist;
				return nullptr;
			}
		}
	}
	
	return dist;
};