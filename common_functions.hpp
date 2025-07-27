#ifndef COMMON_FUNCTIONS_HPP
#define COMMON_FUNCTIONS_HPP

#include "types.hpp"

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

#endif
