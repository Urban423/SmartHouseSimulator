#include "../Graph.h"



template<typename T>
bool isBipartite(IGraph<T>* graph, int* colors) {
    for (int start = 0; start < graph->size(); ++start) {
        if (colors[start] != -1) continue;

        colors[start] = 0;
        int* queue = new int[graph->size()];
        int front = 0, rear = 0;
        queue[rear++] = start;

        while (front < rear) {
            int u = queue[front++];
            for (int v = 0; v < graph->size(); ++v) {
                if (graph->getEdge(u, v) == 1) {
                    if (colors[v] == -1) {
                        colors[v] = 1 - colors[u];
                        queue[rear++] = v;
                    } else if (colors[v] == colors[u]) {
                        delete[] queue;
                        return false;
                    }
                }
            }
        }
        delete[] queue;
    }
    return true;
}

template<typename T>
bool bpm(IGraph<T>* graph, int u, bool* seen, int* matchR) {
    auto it = graph->neighbors(u);
    while (it->hasNext()) {
        auto [v, weight] = it->next();
        if (!seen[v]) {
            seen[v] = true;
            if (matchR[v] == -1 || bpm(graph, matchR[v], seen, matchR)) {
                matchR[v] = u;
                return true;
            }
        }
    }
    return false;
}


template<typename T>
int maxBipartiteMatchingR(IGraph<T>* graph, int* matchR, int* matchingEdges) {
    int matchCount = 0;
    bool* seen = new bool[graph->size()];
    for (int u = 0; u < graph->size(); ++u) {
        std::fill(seen, seen + graph->size(), false);
        if (bpm(graph, u, seen, matchR)) {
            bool isDuplicate = false;
            for (int i = 0; i < matchCount; ++i) {
                if (matchingEdges[i] == u || matchingEdges[i] == matchR[u]) {
                    isDuplicate = true;
                    break;
                }
            }
			if (!isDuplicate) {
				matchingEdges[matchCount++] = u;
			}
		}
	}
	delete[] seen;
	return matchCount;
}

template<typename T>
krray<int> IGraph<T>::maxBipartiteMatching() {
    int* colors = new int[number_of_vertices];
	for (int i = 0; i < number_of_vertices; ++i) colors[i] = -1;
    bool bipartite = isBipartite(this, colors);
	if (!bipartite) {
		delete[] colors;
		return krray<int>();
    }

    for (int i = 0; i < number_of_vertices; ++i) colors[i] = -1;
    int* matchingEdges = new int[number_of_vertices];
    int matchingCount = maxBipartiteMatchingR(this, colors, matchingEdges);

	int* pairs = new int[matchingCount * 2];
	int* ptr = pairs;
	for(int i = 0; i < matchingCount; ++i) {
		*(ptr) = matchingEdges[i];
		++ptr;
		*(ptr) = colors[matchingEdges[i]];
		++ptr;
	}
	krray<int> ret;
	ret.setArray(pairs, matchingCount * 2);
    delete[] colors;
    delete[] matchingEdges;
	return ret;
}
