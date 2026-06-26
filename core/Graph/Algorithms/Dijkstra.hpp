#include "../Graph.h"


template <class T, class Derived>
T* IGraph<T, Derived>::Dijkstra(int start_vertex)
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
        while (it.hasNext()) {
            auto [neighbor, weight] = it.next();
            if (weight != inf && dist[current_vertex] + weight < dist[neighbor]) {
                dist[neighbor] = dist[current_vertex] + weight;
                pq.push({dist[neighbor], neighbor}); // Push the updated distance and vertex into the priority queue
            }
        }
    }

    return dist;
}