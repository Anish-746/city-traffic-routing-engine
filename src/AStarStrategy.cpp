#include "AStarStrategy.hpp"
#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm>
#include <cmath>

static double heuristic(const Node& a, const Node& b) {
    return std::hypot(a.x - b.x, a.y - b.y);
}

RouteResult AStarStrategy::findShortestPath(const Graph& graph, int source, int destination) const {
    struct State {
        int nodeId;
        double fCost; // gCost + hCost
        double gCost; // path cost
        bool operator>(const State& other) const {
            return fCost > other.fCost;
        }
    };

    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    std::unordered_map<int, double> gCosts;
    std::unordered_map<int, int> previous;
    int nodesExpanded = 0;

    const auto& nodes = graph.getNodes();
    
    if (nodes.find(source) == nodes.end() || nodes.find(destination) == nodes.end()) {
        return {{}, 0.0, 0};
    }

    const Node& destNode = graph.getNode(destination);

    gCosts[source] = 0;
    pq.push({source, heuristic(graph.getNode(source), destNode), 0.0});

    while (!pq.empty()) {
        State current = pq.top();
        pq.pop();
        
        if (current.gCost > gCosts[current.nodeId]) continue;
        
        nodesExpanded++;

        if (current.nodeId == destination) break;

        for (const auto& edge : graph.getEdges(current.nodeId)) {
            double newGCost = gCosts[current.nodeId] + edge.baseDistance * edge.trafficMultiplier;
            auto it = gCosts.find(edge.destinationId);
            double currentBestGCost = (it != gCosts.end()) ? it->second : std::numeric_limits<double>::infinity();
            
            if (newGCost < currentBestGCost) {
                gCosts[edge.destinationId] = newGCost;
                previous[edge.destinationId] = current.nodeId;
                double hCost = heuristic(graph.getNode(edge.destinationId), destNode);
                pq.push({edge.destinationId, newGCost + hCost, newGCost});
            }
        }
    }

    auto destIt = gCosts.find(destination);
    if (destIt == gCosts.end() || destIt->second == std::numeric_limits<double>::infinity()) {
        return {{}, 0.0, nodesExpanded};
    }

    std::vector<int> path;
    for (int at = destination; at != source; at = previous[at]) {
        path.push_back(at);
    }
    path.push_back(source);
    std::reverse(path.begin(), path.end());

    return {path, gCosts[destination], nodesExpanded};
}
