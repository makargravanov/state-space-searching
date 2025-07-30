#ifndef ASTAR_HPP
#define ASTAR_HPP

#include "../misc/common_functions.hpp"
#include <queue>
#include <tuple>
#include <cmath>

struct CompareAStar {
    bool operator()(const std::tuple<int, int, Graph::vertex_descriptor>& a, 
                   const std::tuple<int, int, Graph::vertex_descriptor>& b) const {
        return std::get<0>(a) > std::get<0>(b);
    }
};

inline int heuristic(const State& state, int targetVolume) {
    return std::min(std::abs(state.first - targetVolume), 
                   std::abs(state.second - targetVolume));
}

inline auto astar(Graph& graph,
         Graph::vertex_descriptor startVertex,
         std::unordered_map<Graph::vertex_descriptor, State, VertexDescriptorHash>& vertexToState,
         std::unordered_map<State, Graph::vertex_descriptor, StateHash>& stateToVertex,
         int capacityA, int capacityB,
         int targetVolume) -> std::tuple<bool, std::vector<State>, uint32_t> {

    using QueueElement = std::tuple<int, int, Graph::vertex_descriptor>;
    std::priority_queue<QueueElement, 
                        std::vector<QueueElement>,
                        CompareAStar> queue;

    std::unordered_map<Graph::vertex_descriptor, Graph::vertex_descriptor, VertexDescriptorHash> parent;
    std::unordered_map<Graph::vertex_descriptor, int, VertexDescriptorHash> g_score;
    
    g_score[startVertex] = 0;
    int start_f = heuristic(vertexToState[startVertex], targetVolume);
    queue.push(std::make_tuple(start_f, 0, startVertex));
    parent[startVertex] = boost::graph_traits<Graph>::null_vertex();

    bool pathFound = false;
    Graph::vertex_descriptor currentVertex;
    Graph::vertex_descriptor targetVertex = boost::graph_traits<Graph>::null_vertex();

    std::vector<State> nextStates;
    nextStates.reserve(6);
    
    uint32_t visitedNodes = 0;

    while (!queue.empty() && !pathFound) {
        auto [current_f, current_g, currentVertex] = queue.top();
        queue.pop();
        
        if (g_score[currentVertex] < current_g) {
            continue;
        }
        
        visitedNodes++;
        State currentState = vertexToState[currentVertex];

        if (currentState.first == targetVolume || currentState.second == targetVolume) {
            pathFound = true;
            targetVertex = currentVertex;
            break;
        }
 
        generateNextStates(currentState, capacityA, capacityB, nextStates);

        for (const auto& nextState : nextStates) {
            int tentative_g = current_g + 1;
            Graph::vertex_descriptor nextVertex;
            bool isNewVertex = false;
            
            if (stateToVertex.find(nextState) == stateToVertex.end()) {
                nextVertex = boost::add_vertex(graph);
                stateToVertex[nextState] = nextVertex;
                vertexToState[nextVertex] = nextState;
                isNewVertex = true;
            } else {
                nextVertex = stateToVertex[nextState];
            }

            if (isNewVertex || tentative_g < g_score[nextVertex]) {
                if (isNewVertex) {
                    boost::add_edge(currentVertex, nextVertex, graph);
                }
                parent[nextVertex] = currentVertex;
                g_score[nextVertex] = tentative_g;
                int f_score = tentative_g + heuristic(nextState, targetVolume);
                queue.push(std::make_tuple(f_score, tentative_g, nextVertex));
            }
        }
    }

    std::vector<State> path;
    if (pathFound) {
        Graph::vertex_descriptor current = targetVertex;
        while (current != boost::graph_traits<Graph>::null_vertex()) {
            path.push_back(vertexToState[current]);
            current = parent[current];
        }
        std::reverse(path.begin(), path.end());
    }

    return {pathFound, path, visitedNodes};
}

#endif
