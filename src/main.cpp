#include "Graph.hpp"
#include "NavigationContext.hpp"
#include "DijkstraStrategy.hpp"
#include "AStarStrategy.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>

void loadGraph(Graph& graph, const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << filename << std::endl;
        return;
    }

    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        if (line.empty() || line[0] == '#') continue;
        std::stringstream ss(line);
        std::string type;
        std::getline(ss, type, ',');
        
        try {
            if (type == "NODE") {
                Node n;
                std::string idStr, xStr, yStr;
                if (!std::getline(ss, idStr, ',') ||
                    !std::getline(ss, n.name, ',') ||
                    !std::getline(ss, xStr, ',') ||
                    !std::getline(ss, yStr, ',')) {
                    throw std::invalid_argument("Missing fields");
                }
                n.id = std::stoi(idStr);
                n.x = std::stod(xStr);
                n.y = std::stod(yStr);
                graph.addNode(n);
            } else if (type == "EDGE") {
                std::string srcStr, destStr, distStr;
                if (!std::getline(ss, srcStr, ',') ||
                    !std::getline(ss, destStr, ',') ||
                    !std::getline(ss, distStr, ',')) {
                    throw std::invalid_argument("Missing fields");
                }
                graph.addEdge(std::stoi(srcStr), std::stoi(destStr), std::stod(distStr));
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Skipping malformed line " << lineNum << ": " << line << "\n";
        }
    }
}

void printRoute(const RouteResult& res, const std::string& algoName) {
    std::cout << algoName << " Route: ";
    if (res.path.empty()) {
        std::cout << "No path found.\n";
    } else {
        for (size_t i = 0; i < res.path.size(); ++i) {
            std::cout << res.path[i] << (i + 1 == res.path.size() ? "" : " -> ");
        }
        std::cout << "\nTotal Cost: " << std::fixed << std::setprecision(2) << res.totalCost << "\n";
    }
    std::cout << "Nodes Expanded: " << res.nodesExpanded << "\n\n";
}

int main(int argc, char** argv) {
    Graph graph;
    std::string dataPath = "../data/city_graph.csv";
    if (argc > 1) {
        dataPath = argv[1];
    } else {
        std::ifstream test(dataPath);
        if (!test.good()) {
            dataPath = "data/city_graph.csv";
        }
    }
    loadGraph(graph, dataPath);

    std::cout << "City Traffic Routing System CLI\n";
    std::cout << "Commands:\n";
    std::cout << "  simulate_jam <sourceId> <destinationId> <multiplier>\n";
    std::cout << "  compare_route <sourceId> <destinationId>\n";
    std::cout << "  exit\n\n";

    std::string command;
    while (true) {
        std::cout << "> ";
        if (!(std::cin >> command)) break;

        if (command == "exit") {
            break;
        } else if (command == "simulate_jam") {
            int src, dest;
            double multiplier;
            if (std::cin >> src >> dest >> multiplier) {
                try {
                    graph.updateTrafficMultiplier(src, dest, multiplier);
                    std::cout << "Traffic updated for edge " << src << " -> " << dest << " with multiplier " << multiplier << "\n";
                } catch (const std::exception& e) {
                    std::cout << "Error: " << e.what() << "\n";
                }
            } else {
                std::cout << "Invalid arguments for simulate_jam.\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
        } else if (command == "compare_route") {
            int src, dest;
            if (std::cin >> src >> dest) {
                NavigationContext context(std::make_unique<DijkstraStrategy>());
                RouteResult dijkstraRes = context.calculateRoute(graph, src, dest);

                context.setStrategy(std::make_unique<AStarStrategy>());
                RouteResult astarRes = context.calculateRoute(graph, src, dest);

                printRoute(dijkstraRes, "Dijkstra");
                printRoute(astarRes, "A*");
                
                if (dijkstraRes.totalCost == astarRes.totalCost && !dijkstraRes.path.empty()) {
                    std::cout << "Costs match! ";
                    if (astarRes.nodesExpanded <= dijkstraRes.nodesExpanded) {
                        std::cout << "A* is more or equally efficient.\n";
                    } else {
                        std::cout << "Warning: A* expanded more nodes.\n";
                    }
                }
            } else {
                std::cout << "Invalid arguments for compare_route.\n";
                std::cin.clear();
                std::cin.ignore(10000, '\n');
            }
        } else {
            std::cout << "Unknown command.\n";
            std::cin.clear();
            std::cin.ignore(10000, '\n');
        }
    }
    return 0;
}