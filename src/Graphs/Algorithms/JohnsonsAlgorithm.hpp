#include "../Graph.h"

template <class T>
T** IGraph<T>::JohnsonsAlgorithm() {
	IGraph<T>* temp_graph = this->clone();
	temp_graph->addVertecies(1);
	int auxiliary_vertex = temp_graph->number_of_vertices - 1;
	for (int i = 0; i < auxiliary_vertex; ++i) {
		temp_graph->addEdge(auxiliary_vertex, i, 0);
	}
	
	T* h = temp_graph->BellmanFord(auxiliary_vertex);
	if (h == nullptr) {
		delete temp_graph;
		return nullptr;
	}
	
	
	
	// Step 3: Reweight edges in the cloned graph
	for (int u = 0; u < auxiliary_vertex; ++u) {
		auto it = temp_graph->neighbors(u);
		while (it->hasNext()) {
			auto [v, w] = it->next();
			T new_weight = w + h[u] - h[v];
			temp_graph->setEdge(u, v, new_weight);
		}
	}
	

	T** dist = new T*[number_of_vertices];
	for (int u = 0; u < auxiliary_vertex; ++u) {
		dist[u] = temp_graph->Dijkstra(u);
		for (int v = 0; v < auxiliary_vertex; ++v) {
            if (dist[u][v] != inf) {
                dist[u][v] = dist[u][v] - h[u] + h[v];
            }
        }
	}

	delete[] h;
	delete temp_graph;
	return dist;
}