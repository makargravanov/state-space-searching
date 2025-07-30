#ifndef DFS_HPP
#define DFS_HPP

#include "../misc/common_functions.hpp"
#include <stack>

inline auto dfs(Graph& graph,
         Graph::vertex_descriptor startVertex,
         std::unordered_map<Graph::vertex_descriptor, State, VertexDescriptorHash>& vertexToState,
         std::unordered_map<State, Graph::vertex_descriptor, StateHash>& stateToVertex,
         int capacityA, int capacityB,
         int targetVolume) -> std::tuple<bool, std::vector<State>, uint32_t> {

    std::stack<Graph::vertex_descriptor> stack;
    stack.push(startVertex);

    std::unordered_map<Graph::vertex_descriptor, Graph::vertex_descriptor, VertexDescriptorHash> parent;
    parent[startVertex] = boost::graph_traits<Graph>::null_vertex();

    bool pathFound = false;
    Graph::vertex_descriptor currentVertex;

    std::vector<State> nextStates;
    nextStates.reserve(6);
    
    uint32_t visitedNodes = 0;

    while (!stack.empty() && !pathFound) {
        currentVertex = stack.top();
        stack.pop();
        
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
                stack.push(newVertex);
            } else if (parent.find(stateToVertex[nextState]) == parent.end()) {
                parent[stateToVertex[nextState]] = currentVertex;
                stack.push(stateToVertex[nextState]);
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
