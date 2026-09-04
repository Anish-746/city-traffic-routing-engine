#pragma once

#include <vector>

struct RouteResult {
    std::vector<int> path;
    double totalCost;
    int nodesExpanded;
};
