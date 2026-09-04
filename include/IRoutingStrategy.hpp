#pragma once

#include "Graph.hpp"
#include "RouteResult.hpp"

class IRoutingStrategy {
public:
    virtual ~IRoutingStrategy() = default;
    virtual RouteResult findShortestPath(const Graph& graph, int source, int destination) const = 0;
};
