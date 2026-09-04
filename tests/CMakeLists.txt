add_executable(city_routing_tests
    RoutingTests.cpp
)
target_sources(city_routing_tests PRIVATE RoutingTests.cpp)

target_link_libraries(city_routing_tests PRIVATE
    GTest::gtest_main
    city_routing_lib
)

if(ENABLE_SANITIZER)
    target_compile_options(city_routing_tests PRIVATE -fsanitize=address,undefined -g -O1 -fno-omit-frame-pointer)
    target_link_options(city_routing_tests PRIVATE -fsanitize=address,undefined)
endif()

include(GoogleTest)
gtest_discover_tests(city_routing_tests)
