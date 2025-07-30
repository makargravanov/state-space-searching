#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/functional/hash.hpp>
#include <boost/graph/graphviz.hpp>
#include <iostream>
#include <print>
#include <vector>
#include <numeric>

#include "searches/bfs.hpp"
#include "misc/types.hpp"
#include "searches/ucs.hpp"
#include "searches/dfs.hpp"
#include "searches/astar.hpp"


auto main() -> int {
    int capacityA = 4;
    int capacityB = 3;
    int targetVolume = 2;
    bool log = false;
    bool benchmark = false;
    
    std::print("Введите емкость сосуда A: ");
    std::cin >> capacityA;
    std::print("Введите емкость сосуда B: ");
    std::cin >> capacityB;
    std::print("Введите целевой объем: ");
    std::cin >> targetVolume;
    
    std::print("Логи (y/n): ");
    char logc = 'n';
    std::cin >> logc;
    if (logc == 'y') {
        log = true;
    }
    
    std::print("Бенчмарк (y/n): ");
    char benchc = 'n';
    std::cin >> benchc;
    if (benchc == 'y') {
        benchmark = true;
    }
    std::println();

    State initial(capacityA, 0);

    // Списки алгоритмов и их имен
    std::vector<AlgorithmFunction> algorithms = {bfs, ucs, dfs, astar};
    std::vector<std::string> algoNames = {"bfs", "ucs", "dfs", "astar"};
    std::vector<std::string> displayNames = {"BFS", "UCS", "DFS", "A*"};

    if (benchmark) {
        std::vector<std::vector<double>> allTimes(algorithms.size());
        bool allRunsSuccessful = true;

        for (int run = 0; run < 10; ++run) {
            for (size_t i = 0; i < algorithms.size(); ++i) {
                auto [success, time, pathLen, visited] = runAlgorithm(
                    algorithms[i],
                    algoNames[i],
                    initial,
                    capacityA,
                    capacityB,
                    targetVolume,
                    false,  // Для бенчмарка логи отключить надо
                    true // Вообще все логи убираем (тихий режим)
                );

                if (!success) {
                    allRunsSuccessful = false;
                    std::println("Ошибка: алгоритм {} не нашел путь в прогоне {}", displayNames[i], run + 1);
                }
                allTimes[i].push_back(time);
            }
        }

        if (allRunsSuccessful) {
            std::println("=== ОТЧЕТ БЕНЧМАРКА ===");
            for (size_t i = 0; i < algorithms.size(); ++i) {
                double avgTime = std::accumulate(allTimes[i].begin(), allTimes[i].end(), 0.0) / allTimes[i].size();
                std::println("{}: среднее время = {:.6f} сек", displayNames[i], avgTime);
            }
        } else {
            std::println("Бенчмарк не может быть завершен: не все прогоны были успешными");
        }
    } else {
        for (size_t i = 0; i < algorithms.size(); ++i) {
            std::println("=== ЗАПУСК {} ===", displayNames[i]);
            runAlgorithm(
                algorithms[i],
                algoNames[i],
                initial,
                capacityA,
                capacityB,
                targetVolume,
                log,
                false
            );
        }
    }

    return 0;
}
