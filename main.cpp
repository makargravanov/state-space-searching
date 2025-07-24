#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/functional/hash.hpp>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <map>
#include <queue>
#include <vector>
#include <chrono>
#include <algorithm>
#include <print>

using State = std::pair<int, int>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS>;

// Хеш-функция для State (std::pair<int, int>)
struct StateHash {
    size_t operator()(const State& p) const {
        size_t seed = 0;
        boost::hash_combine(seed, p.first);
        boost::hash_combine(seed, p.second);
        return seed;
    }
};

// Хеш-функция для Graph::vertex_descriptor (обычно unsigned long)
struct VertexDescriptorHash {
    size_t operator()(const Graph::vertex_descriptor& v) const {
        return std::hash<Graph::vertex_descriptor>()(v);
    }
};

auto generateNextStates(const State& current,
                        int capacityA,
                        int capacityB) -> std::vector<State> {
    int a = current.first;
    int b = current.second;
    std::vector<State> nextStates;

    nextStates.emplace_back(capacityA, b);
    nextStates.emplace_back(a, capacityB);
    nextStates.emplace_back(0, b);
    nextStates.emplace_back(a, 0);
    
    int transfer = std::min(a, capacityB - b);
    nextStates.emplace_back(a - transfer, b + transfer);
    
    transfer = std::min(b, capacityA - a);
    nextStates.emplace_back(a + transfer, b - transfer);

    return nextStates;
}

auto bfs(Graph& graph,
         Graph::vertex_descriptor startVertex,
         std::unordered_map<Graph::vertex_descriptor, State, VertexDescriptorHash>& vertexToState,
         std::unordered_map<State, Graph::vertex_descriptor, StateHash>& stateToVertex,
         int capacityA, int capacityB,
         int targetVolume) -> std::pair<bool, std::vector<State>> {

    std::queue<Graph::vertex_descriptor> queue;
    queue.push(startVertex);

    std::unordered_map<Graph::vertex_descriptor, Graph::vertex_descriptor, VertexDescriptorHash> parent;
    parent[startVertex] = boost::graph_traits<Graph>::null_vertex();

    bool pathFound = false;
    Graph::vertex_descriptor currentVertex;

    while (!queue.empty() && !pathFound) {
        currentVertex = queue.front();
        queue.pop();

        State currentState = vertexToState[currentVertex];

        if (currentState.first == targetVolume || currentState.second == targetVolume) {
            pathFound = true;
            break;
        }

        std::vector<State> nextStates = generateNextStates(
                                 currentState,
                               capacityA,
                               capacityB);

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

    return {pathFound, path};
}

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
    if(logc == 'y'){
        log = true;
    }
    std::println();
    
    State initial(0, 0);
    Graph graph;
    std::unordered_map<State, Graph::vertex_descriptor, StateHash> stateToVertex;
    std::unordered_map<Graph::vertex_descriptor, State, VertexDescriptorHash> vertexToState;

    Graph::vertex_descriptor startVertex = boost::add_vertex(graph);
    stateToVertex[initial] = startVertex;
    vertexToState[startVertex] = initial;

    auto startTime = 
        std::chrono::high_resolution_clock::now();

    auto [pathFound, path] = 
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

    if (pathFound) {
        std::println("Путь найден ({:.6f} сек):", duration.count());
        if(log){
            for (const auto& state : path) {
                std::println("(A: {}, B: {})", state.first, state.second);
            }
        }
    } else {
        std::println("Решение не найдено ({:.6f} сек).", duration.count());
    }

    return 0;
}
