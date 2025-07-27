#ifndef BFS_HPP
#define BFS_HPP

#include "common_functions.hpp"
#include <queue>

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

    return {pathFound, path, visitedNodes}; 
}

#endif
