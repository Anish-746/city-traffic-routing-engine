#include <gtest/gtest.h>
#include "Graph.hpp"
#include "NavigationContext.hpp"
#include "DijkstraStrategy.hpp"
#include "AStarStrategy.hpp"
#include <memory>

class RoutingTests : public ::testing::Test {
protected:
    void SetUp() override {
        // Simple graph for testing
        // 1(0,0) -10-> 2(0,10)
        // |             |
        // 7             10
        // v             v
        // 3(5,5) -7-> 4(10,10)
        graph.addNode({1, "A", 0, 0});
        graph.addNode({2, "B", 0, 10});
        graph.addNode({3, "C", 5, 5});
        graph.addNode({4, "D", 10, 10});
        graph.addNode({5, "Disconnected", 100, 100});

        graph.addEdge(1, 2, 10.0);
        graph.addEdge(1, 3, 7.0);
        graph.addEdge(2, 4, 10.0);
        graph.addEdge(3, 4, 7.0);
        
        ctx = std::make_unique<NavigationContext>(std::make_unique<DijkstraStrategy>());
    }

    Graph graph;
    std::unique_ptr<NavigationContext> ctx;
};

// 1. Shortest path correctness on a small known graph
TEST_F(RoutingTests, CorrectnessDijkstra) {
    ctx->setStrategy(std::make_unique<DijkstraStrategy>());
    auto result = ctx->calculateRoute(graph, 1, 4);
    
    // Path should be 1 -> 3 -> 4 (cost 14) instead of 1 -> 2 -> 4 (cost 20)
    EXPECT_EQ(result.totalCost, 14.0);
    std::vector<int> expected = {1, 3, 4};
    EXPECT_EQ(result.path, expected);
}

// 2. Disconnected components - confirm empty result, not a crash
TEST_F(RoutingTests, DisconnectedComponents) {
    auto result = ctx->calculateRoute(graph, 1, 5);
    EXPECT_EQ(result.path.size(), 0);
    EXPECT_EQ(result.totalCost, 0.0);
}

// 3. Dynamic recalculation - apply synthetic jam
TEST_F(RoutingTests, DynamicRecalculation) {
    // Original best is 1->3->4 (cost 14)
    // Jam on 3->4 with 3.0 multiplier -> new cost 7 * 3 = 21. Total 1->3->4 is 28.
    // 1->2->4 is 20, which is now better.
    graph.updateTrafficMultiplier(3, 4, 3.0);
    
    auto result = ctx->calculateRoute(graph, 1, 4);
    EXPECT_EQ(result.totalCost, 20.0);
    std::vector<int> expected = {1, 2, 4};
    EXPECT_EQ(result.path, expected);
}

// 4. Strategy consistency & efficiency
TEST_F(RoutingTests, ConsistencyAndEfficiency) {
    ctx->setStrategy(std::make_unique<DijkstraStrategy>());
    auto dijkstraResult = ctx->calculateRoute(graph, 1, 4);
    
    ctx->setStrategy(std::make_unique<AStarStrategy>());
    auto astarResult = ctx->calculateRoute(graph, 1, 4);

    EXPECT_EQ(dijkstraResult.totalCost, astarResult.totalCost);
    EXPECT_EQ(dijkstraResult.path, astarResult.path);
    EXPECT_LE(astarResult.nodesExpanded, dijkstraResult.nodesExpanded);
}

// 5. A* Disconnected components
TEST_F(RoutingTests, AStarDisconnectedComponents) {
    ctx->setStrategy(std::make_unique<AStarStrategy>());
    auto result = ctx->calculateRoute(graph, 1, 5);
    EXPECT_EQ(result.path.size(), 0);
    EXPECT_EQ(result.totalCost, 0.0);
}

// 6. Node removal
TEST_F(RoutingTests, NodeRemoval) {
    graph.removeNode(3);
    
    // Node 3 is gone, so route 1 -> 4 must now go through 2.
    // 1 -> 2 -> 4 (cost 20)
    ctx->setStrategy(std::make_unique<DijkstraStrategy>());
    auto result = ctx->calculateRoute(graph, 1, 4);
    EXPECT_EQ(result.totalCost, 20.0);
    std::vector<int> expected = {1, 2, 4};
    EXPECT_EQ(result.path, expected);
}

// 7. Exception handling in Graph
TEST_F(RoutingTests, GraphExceptions) {
    // Add edge with non-existent nodes
    EXPECT_THROW(graph.addEdge(99, 100, 5.0), std::invalid_argument);
    
    // Update traffic multiplier on non-existent edge
    EXPECT_THROW(graph.updateTrafficMultiplier(1, 4, 2.0), std::invalid_argument);
    
    // Get non-existent node
    EXPECT_THROW(graph.getNode(99), std::out_of_range);
}

// 8. Routing with non-existent source or destination
TEST_F(RoutingTests, RoutingInvalidNodes) {
    ctx->setStrategy(std::make_unique<DijkstraStrategy>());
    auto resultDijkstra = ctx->calculateRoute(graph, 1, 99);
    EXPECT_EQ(resultDijkstra.path.size(), 0);
    
    ctx->setStrategy(std::make_unique<AStarStrategy>());
    auto resultAStar = ctx->calculateRoute(graph, 99, 4);
    EXPECT_EQ(resultAStar.path.size(), 0);
}