#ifndef UCS_HPP
#define UCS_HPP

#include "../misc/common_functions.hpp"
#include <queue>

struct CompareCost {
    bool operator()(const std::pair<int, Graph::vertex_descriptor>& a, 
                    const std::pair<int, Graph::vertex_descriptor>& b) const {
        return a.first > b.first;
    }
};

inline auto ucs(Graph& graph,
         Graph::vertex_descriptor startVertex,
         std::unordered_map<Graph::vertex_descriptor, State, VertexDescriptorHash>& vertexToState,
         std::unordered_map<State, Graph::vertex_descriptor, StateHash>& stateToVertex,
         int capacityA, int capacityB,
         int targetVolume) -> std::tuple<bool, std::vector<State>, uint32_t> {

    std::priority_queue<std::pair<int, Graph::vertex_descriptor>, 
                        std::vector<std::pair<int, Graph::vertex_descriptor>>,
                        CompareCost> queue;
    queue.push({0, startVertex});

    std::unordered_map<Graph::vertex_descriptor, Graph::vertex_descriptor, VertexDescriptorHash> parent;
    std::unordered_map<Graph::vertex_descriptor, int, VertexDescriptorHash> costMap;
    
    parent[startVertex] = boost::graph_traits<Graph>::null_vertex();
    costMap[startVertex] = 0;

    bool pathFound = false;
    Graph::vertex_descriptor targetVertex = boost::graph_traits<Graph>::null_vertex();

    std::vector<State> nextStates;
    nextStates.reserve(6);
    
    uint32_t visitedNodes = 0;

    while (!queue.empty() && !pathFound) {
        auto [currentCost, currentVertex] = queue.top();
        queue.pop();
        
        if (costMap.find(currentVertex) != costMap.end() && costMap[currentVertex] < currentCost) {
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
            int nextCost = currentCost + 1;
            
            if (stateToVertex.find(nextState) == stateToVertex.end()) {
                Graph::vertex_descriptor newVertex = boost::add_vertex(graph);
                stateToVertex[nextState] = newVertex;
                vertexToState[newVertex] = nextState;
                boost::add_edge(currentVertex, newVertex, graph);
                parent[newVertex] = currentVertex;
                costMap[newVertex] = nextCost;
                queue.push({nextCost, newVertex});
            } else {
                Graph::vertex_descriptor nextVertex = stateToVertex[nextState];
                
                if (costMap.find(nextVertex) == costMap.end() || nextCost < costMap[nextVertex]) {
                    parent[nextVertex] = currentVertex;
                    costMap[nextVertex] = nextCost;
                    queue.push({nextCost, nextVertex});
                }
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
