#include "../Graph.h"


template <class T>
T* IGraph<T>::Dijkstra(int start_vertex)
{
    if (number_of_vertices <= start_vertex) { return nullptr; }
    using Pair = std::pair<T, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> pq;
    T* dist = new T[number_of_vertices];
    std::fill(dist, dist + number_of_vertices, inf); 
    dist[start_vertex] = 0;

    pq.push({0, start_vertex});

	
    while (!pq.empty()) {
        T current_dist = pq.top().first;
        int current_vertex = pq.top().second;
        pq.pop();

        if (current_dist > dist[current_vertex]) {
            continue;
        }

        auto it = neighbors(current_vertex);
        while (it->hasNext()) {
            auto [neighbor, weight] = it->next();
            if (weight != inf && dist[current_vertex] + weight < dist[neighbor]) {
                dist[neighbor] = dist[current_vertex] + weight;
                pq.push({dist[neighbor], neighbor}); // Push the updated distance and vertex into the priority queue
            }
        }
    }

    return dist;
}

template <class T>
T* IGraph<T>::Dijkstra1(int start_vertex)
{
	if (number_of_vertices <= start_vertex) { return nullptr; }
	T* dist = new T[number_of_vertices];
	bool* visited = new bool[number_of_vertices];
	FibonacciNode<T, unsigned int>** heaps = new FibonacciNode<T, unsigned int>*[number_of_vertices];
	memset(visited, 0, sizeof(bool) * number_of_vertices);
	memset(heaps, 0, sizeof(FibonacciNode<unsigned int, T>*) * number_of_vertices);
	for (unsigned int i = 0; i < number_of_vertices; ++i) {
		dist[i] = inf;
	}
	dist[start_vertex] = 0;
	FibonacciHeap<T, unsigned int> queue;
	queue.insert(0, start_vertex);
	
	while(!queue.isEmpty()) {
		auto node = queue.extractMin();
		T minDist = node->key;
		int v = node->value;
		
		if(minDist > dist[v]) {
			continue;
		}
		
		if (visited[v]) {
			continue;
		}
		visited[v] = true;
		
		auto it = neighbors(v);
		while (it->hasNext()) {
			auto [u, w] = it->next();
			
			//printf("%d to %d with weight: %d\n", v, u, w);
			if (!visited[u] && dist[v] + w < dist[u]) {
				dist[u] = dist[v] + w;
				if (heaps[u] != nullptr) {
					queue.decreaseKey(heaps[u], dist[u]);
				} 
				else {
					heaps[u] = queue.insert(dist[u], u);
				}
			}
		}
		
	}
	delete[] visited;
	delete[] heaps;
	
	return dist;
	
}