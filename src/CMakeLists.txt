add_library(city_routing_lib
    Graph.cpp
    DijkstraStrategy.cpp
    AStarStrategy.cpp
)

if(ENABLE_SANITIZER)
    target_compile_options(city_routing_lib PRIVATE -fsanitize=address,undefined -g -O1 -fno-omit-frame-pointer)
    target_link_options(city_routing_lib PRIVATE -fsanitize=address,undefined)
endif()

# Initially empty library to make CMake happy before Phase 2
set_target_properties(city_routing_lib PROPERTIES LINKER_LANGUAGE CXX)

add_executable(city_routing_cli
    main.cpp
)

target_link_libraries(city_routing_cli PRIVATE city_routing_lib)

if(ENABLE_SANITIZER)
    target_compile_options(city_routing_cli PRIVATE -fsanitize=address,undefined -g -O1 -fno-omit-frame-pointer)
    target_link_options(city_routing_cli PRIVATE -fsanitize=address,undefined)
endif()
