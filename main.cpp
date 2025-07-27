#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/functional/hash.hpp>
#include <fstream>
#include <boost/graph/graphviz.hpp>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <queue>
#include <vector>
#include <chrono>
#include <algorithm>
#include <print>

using State = std::pair<int, int>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>;

struct StateHash {
    size_t operator()(const State& p) const {
        size_t seed = 0;
        boost::hash_combine(seed, p.first);
        boost::hash_combine(seed, p.second);
        return seed;
    }
};

struct VertexDescriptorHash {
    size_t operator()(const Graph::vertex_descriptor& v) const {
        return std::hash<Graph::vertex_descriptor>()(v);
    }
};

struct VertexWriter {
    const std::unordered_map<Graph::vertex_descriptor, State, VertexDescriptorHash>& vertexToState;
    template <typename Vertex>
    void operator()(std::ostream& out, const Vertex& v) const {
        auto it = vertexToState.find(v);
        if (it != vertexToState.end()) {
            out << "[label=\"(" << it->second.first << "," << it->second.second << ")\"]";
        } else {
            out << "[label=\"?\"]";
        }
    }
};

inline auto generateNextStates(const State& current,
                        int capacityA,
                        int capacityB,
                        std::vector<State>& nextStates) -> void {
    int a = current.first;
    int b = current.second;
    nextStates.clear();
    nextStates.emplace_back(capacityA, b);
    nextStates.emplace_back(a, capacityB);
    nextStates.emplace_back(0, b);
    nextStates.emplace_back(a, 0);
    
    int transfer = std::min(a, capacityB - b);
    nextStates.emplace_back(a - transfer, b + transfer);
    
    transfer = std::min(b, capacityA - a);
    nextStates.emplace_back(a + transfer, b - transfer);

}

inline auto bfs(Graph& graph,
         Graph::vertex_descriptor startVertex,
         std::unordered_map<Graph::vertex_descriptor, State, VertexDescriptorHash>& vertexToState,
         std::unordered_map<State, Graph::vertex_descriptor, StateHash>& stateToVertex,
         int capacityA, int capacityB,
         int targetVolume) -> std::tuple<bool, std::vector<State>, uint32_t> {

    std::queue<Graph::vertex_descriptor> queue;
    queue.push(startVertex);

    std::unordered_map<Graph::vertex_descriptor, Graph::vertex_descriptor, VertexDescriptorHash> parent;
    
    parent[startVertex] = boost::graph_traits<Graph>::null_vertex();

    bool pathFound = false;
    Graph::vertex_descriptor currentVertex;

    std::vector<State> nextStates;
    nextStates.reserve(6);
    
    uint32_t visitedNodes = 0;

    while (!queue.empty() && !pathFound) {
        currentVertex = queue.front();
        queue.pop();
        
        visitedNodes++;

        State currentState = vertexToState[currentVertex];

        if (currentState.first == targetVolume || currentState.second == targetVolume) {
            pathFound = true;
            break;
        }
 
        generateNextStates(currentState, capacityA, capacityB, nextStates);

        for (const auto& nextState : nextStates) {
            if (stateToVertex.find(nextState) == stateToVertex.end()) {
                Graph::vertex_descriptor newVertex = boost::add_vertex(graph);
                stateToVertex[nextState] = newVertex;
                vertexToState[newVertex] = nextState;
                boost::add_edge(currentVertex, newVertex, graph);
                parent[newVertex] = currentVertex;
                queue.push(newVertex);
            } else if (parent.find(stateToVertex[nextState]) == parent.end()) {
                parent[stateToVertex[nextState]] = currentVertex;
                queue.push(stateToVertex[nextState]);
            }
        }
    }

    std::vector<State> path;
    if (pathFound) {
        Graph::vertex_descriptor current = currentVertex;
        while (current != boost::graph_traits<Graph>::null_vertex()) {
            path.push_back(vertexToState[current]);
            current = parent[current];
        }
        std::reverse(path.begin(), path.end());
    }

    return {pathFound, path, visitedNodes}; }

auto main() -> int {
    int capacityA    = 4;
    int capacityB    = 3;
    int targetVolume = 2;
    bool log         = false;
    
    std::print("Введите емкость сосуда A: ");
    std::cin >> capacityA;
    std::print("Введите емкость сосуда B: ");
    std::cin >> capacityB;
    std::print("Введите целевой объем: ");
    std::cin >> targetVolume;
    std::print("Логи (y/n): ");
    char logc = 'n';
    std::cin >> logc;
    if(logc == 'y') {
        log = true;
    }
    std::println();
    
    State initial(capacityA, 0);
    Graph graph;
    std::unordered_map<State, Graph::vertex_descriptor, StateHash> stateToVertex;
    std::unordered_map<Graph::vertex_descriptor, State, VertexDescriptorHash> vertexToState;

    Graph::vertex_descriptor startVertex = boost::add_vertex(graph);
    stateToVertex[initial] = startVertex;
    vertexToState[startVertex] = initial;

    auto startTime = 
        std::chrono::high_resolution_clock::now();

    
    auto [pathFound, path, visitedNodes] = 
        bfs(graph,
            startVertex,
            vertexToState,
            stateToVertex,
            capacityA,
            capacityB,
            targetVolume
        );

    auto endTime = 
        std::chrono::high_resolution_clock::now();

    auto duration = 
    std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);

    double directedness = pathFound ? static_cast<double>(path.size()) / visitedNodes : 0.0;

    if (pathFound) {
        std::println("Путь найден ({:.6f} сек):", duration.count());
        std::println("Длина пути: {}", path.size());
        std::println("Посещено узлов: {}", visitedNodes);
        std::println("Целенаправленность: {:.4f}", directedness);

        if (log) {
            std::ofstream dot_file("bfs_search_tree.dot");
            if (dot_file.is_open()) {
                boost::write_graphviz(dot_file, graph, VertexWriter{vertexToState});
                dot_file.close();
                std::println("\nГраф дерева перебора сохранен в файл bfs_search_tree.dot");
                std::println("Для визуализации выполните в терминале:");
                std::println("dot -Tpng bfs_search_tree.dot -o tree.png && feh tree.png");
            } else {
                std::cerr << "Ошибка: не удалось создать файл search_tree.dot\n";
            }
        }
    } else {
        std::println("Решение не найдено ({:.6f} сек).", duration.count());
    }
    

    return 0;
}
