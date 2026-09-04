# City Traffic Routing Engine 

A high-performance C++20 traffic routing simulator that models a city as a graph of intersections and roads, supporting dynamic traffic conditions.

## Overview
This project simulates city traffic routing by representing a city as a directed graph. It allows users to query the shortest paths between intersections and simulate real-time traffic jams. It is built to demonstrate modern C++ features, object-oriented design principles (RAII, Strategy Pattern, const-correctness), and efficient algorithm implementations.

## Key Features
- **Dynamic Routing**: Query shortest paths dynamically using interchangeable algorithms.
- **Strategy Pattern**: Swap between Dijkstra's and A* search algorithms seamlessly at runtime.
- **Real-Time Traffic Jams**: Inject traffic multipliers onto specific roads to observe how optimal routes adapt on the fly.
- **Graph Modeling**: Custom ID-based graph ownership model ensuring clean memory management without over-relying on smart pointers.
- **Memory Safe**: Configured and verified with AddressSanitizer (ASan) and UndefinedBehaviorSanitizer (UBSan).

## Tech Stack
- **Language**: C++20
- **Build System**: CMake (3.14+)
- **Testing**: Google Test (GTest) integrated via FetchContent
- **Memory Tools**: AddressSanitizer (ASan), UndefinedBehaviorSanitizer (UBSan)

## Folder Structure
```text
city-traffic-routing-system/
├── CMakeLists.txt        # Root CMake configuration
├── data/
│   └── city_graph.csv    # Sample city graph data
├── include/              # Public header files (.hpp)
├── src/                  # Implementation files (.cpp) and main entry point
└── tests/                # GTest unit tests
```

## Setup Instructions

### Prerequisites
- A C++20 compatible compiler (GCC 10+, Clang 10+, or MSVC)
- CMake 3.14 or newer
- Make or Ninja

### Building the Project
1. Clone the repository and navigate to the root directory.
2. Create a build directory:
   ```bash
   mkdir build && cd build
   ```
3. Configure the project with CMake. To enable memory sanitizers, pass `-DENABLE_SANITIZER=ON`:
   ```bash
   cmake -DENABLE_SANITIZER=ON ..
   ```
4. Build the project:
   ```bash
   make -j$(nproc)
   ```

### Running the Simulator CLI
Run the compiled executable from the build directory:
```bash
./city_routing_cli
```
You can also pass a custom CSV graph file as an argument:
```bash
./city_routing_cli ../data/custom_graph.csv
```

**Available CLI Commands:**
- `compare_route <sourceId> <destinationId>`: Calculates and compares the route using both Dijkstra and A*.
- `simulate_jam <sourceId> <destinationId> <multiplier>`: Applies a traffic multiplier to an edge (e.g. `simulate_jam 3 4 3.0` makes the road from 3 to 4 three times slower).
- `exit`: Quits the application.

### Running Tests
To run the automated test suite (which also runs under sanitizers if enabled):
```bash
cd build
ctest --output-on-failure
```