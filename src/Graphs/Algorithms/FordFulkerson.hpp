#include "../Graph.h"


template <class T>
int IGraph<T>::FordFulkerson(int source, int sink) {
	auto residualGraph = this->clone();  
	int max_flow = 0;
	int* parent = new int[this->number_of_vertices];
	T** flow = new T*[number_of_vertices];
	for (int i = 0; i < number_of_vertices; ++i) {
		flow[i] = new T[number_of_vertices]{0};
	}
	while (BFS(residualGraph, source, sink, parent)) {
		int path_flow = 0x7f'ff'ff'ff;
		for (int v = sink; v != source; v = parent[v]) {
			int u = parent[v];
			path_flow = std::min(path_flow, (int)residualGraph->getEdge(u, v));
		}
		for (int v = sink; v != source; v = parent[v]) {
			int u = parent[v];
			flow[u][v] += path_flow;
			flow[v][u] -= path_flow;
			residualGraph->setEdge(u, v, residualGraph->getEdge(u, v) - path_flow);
			residualGraph->setEdge(v, u, residualGraph->getEdge(v, u) + path_flow);
		}
		max_flow += path_flow;
	}

	//print
	printf("a) Maximum Flow: %d\nb) Flow and Capacity for each edge:\n", max_flow);
	int index = 1;
	for (int u = 0; u < number_of_vertices; ++u) {
		for (int v = 0; v < number_of_vertices; ++v) {
			if (u == v) continue;
			T capacity = this->getEdge(u, v);
			if (capacity > 0 && capacity != inf && flow[u][v] != 0) {
				printf("%d) Edge (%d -> %d) Flow: %d\n", index++, u, v, flow[u][v]);
			}
		}
	}
	
	//clear
	delete residualGraph;
	delete[] parent;
	for (int i = 0; i < number_of_vertices; ++i) { delete[] flow[i]; }
	delete[] flow;

	return max_flow;
}





template <class T>
bool IGraph<T>::BFS(IGraph<T>* residualGraph, int source, int sink, int* parent) {
    bool* visited = new bool[number_of_vertices]();
    int* queue = new int[number_of_vertices];
    int front = 0, rear = 0;
	
    queue[rear++] = source;
    visited[source] = true;
    parent[source] = -1;

    while (front < rear) {
        int u = queue[front++];
        auto it = residualGraph->neighbors(u);
        while (it->hasNext()) {
            auto [v, capacity] = it->next();
            if (!visited[v] && capacity > 0) {
                queue[rear++] = v;
                parent[v] = u;
                visited[v] = true;
                if (v == sink) {
                    delete[] visited;
                    delete[] queue;
                    return true;
                }
            }
        }
    }
	
	
    delete[] visited;
    delete[] queue;
    return false;
}