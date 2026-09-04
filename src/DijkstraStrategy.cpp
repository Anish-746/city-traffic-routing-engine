#include "DijkstraStrategy.hpp"
#include <queue>
#include <unordered_map>
#include <limits>
#include <algorithm>

RouteResult DijkstraStrategy::findShortestPath(const Graph& graph, int source, int destination) const {
    struct State {
        int nodeId;
        double cost;
        bool operator>(const State& other) const {
            return cost > other.cost;
        }
    };

    std::priority_queue<State, std::vector<State>, std::greater<State>> pq;
    std::unordered_map<int, double> costs;
    std::unordered_map<int, int> previous;
    int nodesExpanded = 0;

    const auto& nodes = graph.getNodes();
    
    // Check if source and destination exist in the graph
    if (nodes.find(source) == nodes.end() || nodes.find(destination) == nodes.end()) {
        return {{}, 0.0, 0};
    }

    costs[source] = 0;
    pq.push({source, 0});

    while (!pq.empty()) {
        State current = pq.top();
        pq.pop();

        if (current.cost > costs[current.nodeId]) continue;
        
        nodesExpanded++;

        if (current.nodeId == destination) break;

        for (const auto& edge : graph.getEdges(current.nodeId)) {
            double newCost = costs[current.nodeId] + edge.baseDistance * edge.trafficMultiplier;
            auto it = costs.find(edge.destinationId);
            double currentBestCost = (it != costs.end()) ? it->second : std::numeric_limits<double>::infinity();
            
            if (newCost < currentBestCost) {
                costs[edge.destinationId] = newCost;
                previous[edge.destinationId] = current.nodeId;
                pq.push({edge.destinationId, newCost});
            }
        }
    }

    auto destIt = costs.find(destination);
    if (destIt == costs.end() || destIt->second == std::numeric_limits<double>::infinity()) {
        return {{}, 0.0, nodesExpanded};
    }

    std::vector<int> path;
    for (int at = destination; at != source; at = previous[at]) {
        path.push_back(at);
    }
    path.push_back(source);
    std::reverse(path.begin(), path.end());

    return {path, costs[destination], nodesExpanded};
}
