cmake_minimum_required(VERSION 3.20)

# Project name
project(o3_system)

# C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Directories
set(SRC_DIR src)
set(INCLUDE_DIR include)
set(VISUALIZER_DIR visualizer)
set(EXAMPLES_DIR examples)

# Source files for the shared library
set(LIB_SRCS
    ${SRC_DIR}/neuron.cpp
    ${SRC_DIR}/synapse.cpp
    ${SRC_DIR}/neuron_gate.cpp
    ${SRC_DIR}/network.cpp
    ${SRC_DIR}/utils.cpp
    ${VISUALIZER_DIR}/visualizer.cpp
)

# Include directories
include_directories(${INCLUDE_DIR})

# Build the shared library
add_library(o3_shared SHARED ${LIB_SRCS})
target_include_directories(o3_shared PUBLIC ${INCLUDE_DIR})

# Example executables
add_executable(simple_network ${EXAMPLES_DIR}/simple_network.cpp)
target_link_libraries(simple_network o3_shared)

add_executable(pathway_generation ${EXAMPLES_DIR}/pathway_generation.cpp)
target_link_libraries(pathway_generation o3_shared)

# Main executable (optional, if needed)
add_executable(${PROJECT_NAME} ${SRC_DIR}/main.cpp)
target_link_libraries(${PROJECT_NAME} o3_shared)