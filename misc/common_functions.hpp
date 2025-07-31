#ifndef COMMON_FUNCTIONS_HPP
#define COMMON_FUNCTIONS_HPP

#include "types.hpp"
#include <print>

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

inline std::tuple<bool, double, size_t, uint32_t> runAlgorithm(
    AlgorithmFunction algo,
    const std::string& algoName,
    const State& initial,
    int capacityA,
    int capacityB,
    int targetVolume,
    bool log,
    bool silent = false
) {
    Graph graph;
    std::unordered_map<State, Graph::vertex_descriptor, StateHash> stateToVertex;
    std::unordered_map<Graph::vertex_descriptor, State, VertexDescriptorHash> vertexToState;

    Graph::vertex_descriptor startVertex = boost::add_vertex(graph);
    stateToVertex[initial] = startVertex;
    vertexToState[startVertex] = initial;

    auto startTime = std::chrono::high_resolution_clock::now();
    auto [pathFound, path, visitedNodes] = algo(
        graph,
        startVertex,
        vertexToState,
        stateToVertex,
        capacityA,
        capacityB,
        targetVolume
    );
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    double time = duration.count();

    if (!silent) {
        if (pathFound) {
            std::println("Путь найден ({:.6f} сек):", time);
            std::println("Длина пути: {}", path.size());
            std::println("Посещено узлов: {}", visitedNodes);
            std::println("Целенаправленность: {:.4f}", static_cast<double>(path.size()) / visitedNodes);

            if (log) {
                std::string filename = algoName + "_search_tree.dot";
                std::ofstream dot_file(filename);

                std::println("Путь: ");
                for (const auto& e : path) {
                    std::println("{}", e);
                }
                std::println(" ");

                if (dot_file.is_open()) {
                    boost::write_graphviz(dot_file, graph, VertexWriter{vertexToState});
                    dot_file.close();
                    std::println("\nГраф дерева перебора сохранен в файл {}", filename);
                    std::println("Для визуализации выполните в терминале:");
                    std::println("dot -Tpng {} -o {}_tree.png && feh {}_tree.png", filename, algoName, algoName);
                } else {
                    std::cerr << "Ошибка: не удалось создать файл " << filename << "\n";
                }
            }
        } else {
            std::println("Решение не найдено ({:.6f} сек).", time);
        }
        std::println();
    }

    return {pathFound, time, path.size(), visitedNodes};
}


#endif
