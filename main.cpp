#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/functional/hash.hpp>
#include <fstream>
#include <boost/graph/graphviz.hpp>
#include <iostream>
#include <chrono>
#include <print>

#include "bfs.hpp"


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
