#pragma once

#include "Node.hpp"
#include "Edge.hpp"
#include <unordered_map>
#include <vector>

class Graph {
public:
    void addNode(const Node& node);
    void removeNode(int id);
    void addEdge(int sourceId, int destinationId, double baseDistance);
    void updateTrafficMultiplier(int sourceId, int destinationId, double multiplier);

    const Node& getNode(int id) const;
    const std::vector<Edge>& getEdges(int id) const;
    const std::unordered_map<int, Node>& getNodes() const;

    std::unordered_map<int, Node> nodes_;
    std::unordered_map<int, std::vector<Edge>> adjacencyList_;
    std::unordered_map<int, std::vector<int>> incomingEdges_;
};
