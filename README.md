# O3 (Ozone) Neuromorphic Architecture

A C++ implementation of a neuromorphic computing system that simulates biological neurons and their communication pathways.

## Overview

O3 is a dynamic neuromorphic intelligence architecture that replicates how biological neurons compute, understand data, and communicate with each other. Unlike traditional machine learning approaches, this system focuses on simulating individual neurons and their interactions.

## Key Components

- **Neurons**: Simulates biological neurons with various specializations
- **Synapses**: Handles data transfer between neurons 
- **Neuron Gates**: Controls signal processing within neurons
- **Networks**: Manages collections of neurons and their connections

## Architecture Tiers

1. **Conscious**: High-level cognitive functions with attention focus
2. **Subconscious**: Pattern recognition and routine processing  
3. **Unconscious**: Basic functions, reflexes, and deep memory

## Building the Project

The project uses CMake as its build system. To build:

```sh
mkdir build
cd build
cmake ..
make
```

## Project Structure

```markdown
├── CMakeLists.txt
├── include/
│   ├── network.h
│   ├── neuron_gate.h  
│   ├── neuron.h
│   ├── synapse.h
│   └── utils.h
├── src/
│   ├── main.cpp
│   ├── network.cpp
│   ├── neuron_gate.cpp
│   ├── neuron.cpp
│   ├── synapse.cpp
│   └── utils.cpp
├── examples/
│   ├── pathway_generation.cpp
│   └── simple_network.cpp
└── visualizer/
    └── visualizer.cpp
```

## Usage
The system provides an interactive console interface with the following options:

     1.Run Simple Network Example
     2.Run Pathway Generation Example
     3.Show Architecture Info

You can also run examples directly using command line arguments:
```
     ./o3_system simple    # Run simple network example
     ./o3_system pathway   # Run pathway generation example

```

## Development
The project is structured as a shared library (o3_shared) with example executables that demonstrate its functionality. The code follows C++11 standards and is built with Clang/LLVM.

Requirements
CMake 3.20 or higher
C++11 compatible compiler
macOS 11.7 or higher (for current build configuration)