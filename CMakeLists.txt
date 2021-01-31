cmake_minimum_required(VERSION 3.6)
project(lisp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")
set (CMAKE_CXX_STANDARD 20)
find_package(GTest REQUIRED)
find_package(benchmark REQUIRED)

include_directories(include)

# Link runTests with what we want to test and the GTest and pthread library
add_executable(lisp src/main.cpp src/atom.cpp)
target_link_libraries(lisp)

include_directories(${GTEST_INCLUDE_DIRS})
# Link runTests with what we want to test and the GTest library
add_executable(runTests test/tests.cpp src/atom.cpp)
target_link_libraries(runTests ${GTEST_LIBRARIES} gtest_main)

add_executable(runBench test/bench.cpp src/atom.cpp)
target_link_libraries(runBench benchmark::benchmark)
