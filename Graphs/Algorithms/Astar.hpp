#include "../Graph.h"


template <class T>
PathData<T> IGraph<T>::Astar(int start, int end, T (*heuristic)(int, int)) {
	if (number_of_vertices <= start || number_of_vertices <= end) { return {nullptr, nullptr, 0}; }

    using Pair = std::pair<T, int>;
    std::priority_queue<Pair, std::vector<Pair>, std::greater<Pair>> pq;
    T* dist = new T[number_of_vertices];
    int* came_from = new int[number_of_vertices];
    std::fill(dist, dist + number_of_vertices, inf);
    std::fill(came_from, came_from + number_of_vertices, -1);
    dist[start] = 0;
	
    pq.push({heuristic(start, end), start});
    
	while (!pq.empty()) {
        T current_f = pq.top().first;
        int current_vertex = pq.top().second;
        pq.pop();

        if (current_vertex == end) {
            return  {dist, came_from, 0};
        }

        auto it = neighbors(current_vertex);
        while (it->hasNext()) {
            auto [neighbor, weight] = it->next();
            if (weight != inf) {
                T tentative_g_score = dist[current_vertex] + weight;
                if (tentative_g_score < dist[neighbor]) { 
                    dist[neighbor] = tentative_g_score;
                    came_from[neighbor] = current_vertex;
                    T f_score = dist[neighbor] + heuristic(neighbor, end);
                    pq.push({f_score, neighbor});
                }
            }
        }
    }
	delete[] dist;
	delete[] came_from;
	return {nullptr, nullptr, 0};
}