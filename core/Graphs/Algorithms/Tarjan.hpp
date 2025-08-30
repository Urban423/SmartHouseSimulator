#include "../Graph.h"
#include <algorithm>

template<class T>
void IGraph<T>::Tarjan() {
    std::vector<int> disc(number_of_vertices, -1);   // Discovery time of vertices
    std::vector<int> low(number_of_vertices, -1);    // Lowest discovery reachable
    std::vector<int> parent(number_of_vertices, -1); // Parent of each vertex in DFS tree
    std::vector<bool> is_cut_vertex(number_of_vertices, false); // Cut vertex flags
    std::stack<int> stack;                          // Stack for Tarjan
    std::vector<bool> in_stack(number_of_vertices, false); // Stack membership flag
    std::vector<std::pair<int, int>> bridges;       // List of bridges
    std::vector<std::vector<int>> biconnected_components; // List of biconnected components
    std::vector<std::vector<int>> connected_components;   // List of connected components
    std::vector<bool> visited(number_of_vertices, false); // Visited vertices for connected components

    int time = 0;

    auto strongconnect = [&](int v, auto& self) -> void {
        disc[v] = low[v] = time++;
        stack.push(v);
        in_stack[v] = true;

        std::unique_ptr<NeighborIterator> iter = neighbors(v);

        while (iter->hasNext()) {
            auto [w, weight] = iter->next();
            if (disc[w] == -1) { // If w is not visited
                parent[w] = v;

                self(w, self);

                // Update low value
                low[v] = std::min(low[v], low[w]);

                // Check for bridge
                if (low[w] > disc[v]) {
                    bridges.emplace_back(v, w);
                }

                // Check for cut vertex
                if (low[w] >= disc[v] && parent[v] != -1) {
                    is_cut_vertex[v] = true;
                }

            } else if (w != parent[v] && in_stack[w]) { // Back edge
                low[v] = std::min(low[v], disc[w]);
            }
        }

        // Root of DFS tree is a cut vertex if it has two or more children
        if (parent[v] == -1 && std::count(parent.begin(), parent.end(), v) > 1) {
            is_cut_vertex[v] = true;
        }

        // If v is a root or forms a biconnected component
        if (low[v] == disc[v]) {
            std::vector<int> bcc;
            while (!stack.empty()) {
                int u = stack.top();
                stack.pop();
                in_stack[u] = false;
                bcc.push_back(u);
                if (u == v) break;
            }
            biconnected_components.push_back(bcc);
        }
    };

    for (int v = 0; v < number_of_vertices; ++v) {
        if (disc[v] == -1) {
            std::vector<int> current_component;
            strongconnect(v, strongconnect);
            connected_components.push_back(current_component);
        }
    }

    // Output results
    std::cout << "Bridges:\n";
    for (const auto& bridge : bridges) {
        std::cout << "(" << bridge.first << ", " << bridge.second << ")\n";
    }

    std::cout << "Cut vertices:\n[";
    for (int i = 0; i < number_of_vertices; ++i) {
        if (is_cut_vertex[i]) {
            std::cout << i << " ";
        }
    }
    std::cout << "]\n";

    std::cout << "Biconnected components:\n";
    for (const auto& component : biconnected_components) {
        std::cout << "[";
        for (size_t i = 0; i < component.size(); ++i) {
            std::cout << component[i] << (i + 1 < component.size() ? ", " : "");
        }
        std::cout << "]\n";
    }

    std::cout << "Connected components:\n";
    for (const auto& component : connected_components) {
        std::cout << "[";
        for (size_t i = 0; i < component.size(); ++i) {
            std::cout << component[i] << (i + 1 < component.size() ? ", " : "");
        }
        std::cout << "]\n";
    }
}
