#include "Graph.hpp"
#include <algorithm>
#include <stdexcept>

void Graph::addNode(const Node& node) {
    nodes_[node.id] = node;
}

void Graph::removeNode(int id) {
    nodes_.erase(id);
    
    // Clean up incomingEdges_ for nodes this node points to
    auto adjIt = adjacencyList_.find(id);
    if (adjIt != adjacencyList_.end()) {
        for (const auto& edge : adjIt->second) {
            auto& inEdges = incomingEdges_[edge.destinationId];
            inEdges.erase(std::remove(inEdges.begin(), inEdges.end(), id), inEdges.end());
        }
        adjacencyList_.erase(adjIt);
    }

    // Remove edges pointing to this node from other nodes' adjacency lists
    auto inIt = incomingEdges_.find(id);
    if (inIt != incomingEdges_.end()) {
        for (int sourceId : inIt->second) {
            auto& edges = adjacencyList_[sourceId];
            edges.erase(
                std::remove_if(edges.begin(), edges.end(),
                               [id](const Edge& e) { return e.destinationId == id; }),
                edges.end());
        }
        incomingEdges_.erase(inIt);
    }
}

void Graph::addEdge(int sourceId, int destinationId, double baseDistance) {
    if (nodes_.find(sourceId) == nodes_.end() || nodes_.find(destinationId) == nodes_.end()) {
        throw std::invalid_argument("Source or destination node does not exist.");
    }
    adjacencyList_[sourceId].push_back({destinationId, baseDistance, 1.0});
    incomingEdges_[destinationId].push_back(sourceId);
}

void Graph::updateTrafficMultiplier(int sourceId, int destinationId, double multiplier) {
    auto it = adjacencyList_.find(sourceId);
    if (it != adjacencyList_.end()) {
        for (auto& edge : it->second) {
            if (edge.destinationId == destinationId) {
                edge.trafficMultiplier = multiplier;
                return;
            }
        }
    }
    throw std::invalid_argument("Edge does not exist.");
}

const Node& Graph::getNode(int id) const {
    auto it = nodes_.find(id);
    if (it == nodes_.end()) {
        throw std::out_of_range("Node not found.");
    }
    return it->second;
}

const std::vector<Edge>& Graph::getEdges(int id) const {
    static const std::vector<Edge> empty_edges;
    auto it = adjacencyList_.find(id);
    if (it == adjacencyList_.end()) {
        return empty_edges;
    }
    return it->second;
}

const std::unordered_map<int, Node>& Graph::getNodes() const {
    return nodes_;
}
