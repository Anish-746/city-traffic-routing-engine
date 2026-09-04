#pragma once

#include "IRoutingStrategy.hpp"
#include <memory>
#include <stdexcept>

class NavigationContext {
public:
    explicit NavigationContext(std::unique_ptr<IRoutingStrategy> strategy)
        : strategy_(std::move(strategy)) {}

    void setStrategy(std::unique_ptr<IRoutingStrategy> strategy) {
        strategy_ = std::move(strategy);
    }

    RouteResult calculateRoute(const Graph& graph, int source, int destination) const {
        if (!strategy_) {
            throw std::runtime_error("Routing strategy not set");
        }
        return strategy_->findShortestPath(graph, source, destination);
    }

private:
    std::unique_ptr<IRoutingStrategy> strategy_;
};
