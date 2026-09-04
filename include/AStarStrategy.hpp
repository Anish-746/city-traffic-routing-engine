#pragma once

#include "IRoutingStrategy.hpp"

class AStarStrategy : public IRoutingStrategy {
public:
    RouteResult findShortestPath(const Graph& graph, int source, int destination) const override;
};
