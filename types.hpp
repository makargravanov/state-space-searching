#ifndef TYPES_HPP
#define TYPES_HPP

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/functional/hash.hpp>
#include <boost/graph/graphviz.hpp>
#include <iostream>
#include <unordered_map>
#include <utility>

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

using AlgorithmFunction = std::tuple<bool, std::vector<State>, uint32_t> (*)(
    Graph&,
    Graph::vertex_descriptor,
    std::unordered_map<Graph::vertex_descriptor, State, VertexDescriptorHash>&,
    std::unordered_map<State, Graph::vertex_descriptor, StateHash>&,
    int, int, int
);

#endif
