/**
 * @file main.cpp
 * @brief Main entry point for the Ozone (O3) neuromorphic architecture simulation.
 * 
 * This file contains the main function that initializes the system, sets up
 * a basic neuromorphic network for demonstration, and provides a simple
 * interactive console to engage with the network.
 */

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <csignal>

#include "../include/neuron.h"
#include "../include/synapse.h"
#include "../include/neuron_gate.h"
#include "../include/network.h"
#include "../include/utils.h"
#include "../visualizer/visualizer.h"

// Global flag for shutdown signal
volatile std::sig_atomic_t g_shutdown = 0;

// Signal handler for graceful shutdown
void signalHandler(int signum) {
    std::cout << "\nInterrupt signal received. Shutting down gracefully..." << std::endl;
    g_shutdown = 1;
}

// Forward declaration for example functions
void runSimpleNetworkExample();
void runPathwayGenerationExample();

/**
 * Display the main menu and get user selection
 */
int showMenu() {
    int choice = 0;
    
    std::cout << "\n=== Ozone (O3) Neuromorphic Architecture ===\n";
    std::cout << "1. Run Simple Network Example\n";
    std::cout << "2. Run Pathway Generation Example\n";
    std::cout << "3. Show Architecture Info\n";
    std::cout << "0. Exit\n";
    std::cout << "Enter choice: ";
    
    std::cin >> choice;
    
    // Clear input buffer
    std::cin.clear();
    std::cin.ignore(10000, '\n');
    
    return choice;
}

/**
 * Display information about the architecture
 */
void showArchitectureInfo() {
    std::cout << "\n=== Ozone (O3) Dynamic Neuromorphic Intelligence Architecture ===\n";
    std::cout << "This is a neuromorphic computing system that simulates biological\n";
    std::cout << "neurons and their communication pathways. Unlike traditional machine\n";
    std::cout << "learning approaches, this architecture replicates how individual\n";
    std::cout << "neurons compute, understand data, and communicate with each other.\n\n";
    
    std::cout << "Key components:\n";
    std::cout << "- Neurons: Simulate biological neurons with various specializations\n";
    std::cout << "- Synapses: Handle data transfer between neurons\n";
    std::cout << "- Neuron Gates: Control signal processing within neurons\n";
    std::cout << "- Networks: Manage collections of neurons and their connections\n\n";
    
    std::cout << "The architecture is organized into three tiers:\n";
    std::cout << "1. Conscious: High-level cognitive functions with attention focus\n";
    std::cout << "2. Subconscious: Pattern recognition and routine processing\n";
    std::cout << "3. Unconscious: Basic functions, reflexes, and deep memory\n\n";
    
    std::cout << "Press Enter to continue...";
    std::cin.get();
}

/**
 * Main function
 */
int main(int argc, char* argv[]) {
    // Set up signal handler for Ctrl+C
    std::signal(SIGINT, signalHandler);
    
    // Seed random number generator
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    
    if (argc > 1) {
        // If command line arguments provided, run the specified example
        std::string arg = argv[1];
        
        if (arg == "simple") {
            runSimpleNetworkExample();
            return 0;
        } else if (arg == "pathway") {
            runPathwayGenerationExample();
            return 0;
        }
    }
    
    // Interactive menu loop
    while (!g_shutdown) {
        int choice = showMenu();
        
        switch (choice) {
            case 1:
                runSimpleNetworkExample();
                break;
                
            case 2:
                runPathwayGenerationExample();
                break;
                
            case 3:
                showArchitectureInfo();
                break;
                
            case 0:
                std::cout << "Exiting..." << std::endl;
                g_shutdown = 1;
                break;
                
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
                break;
        }
    }
    
    return 0;
}

/**
 * Run the simple network example
 */
void runSimpleNetworkExample() {
    std::cout << "\n=== Running Simple Network Example ===\n";
    
    // The example should be run from the example file
    std::cout << "Executing simple_network.cpp example...\n" << std::endl;
    
    // Since main.cpp is the actual entry point, we're just forwarding to the example
    // This would normally be done via the command line or a separate executable
    extern int simpleNetworkMain();
    simpleNetworkMain();
}

/**
 * Run the pathway generation example
 */
void runPathwayGenerationExample() {
    std::cout << "\n=== Running Pathway Generation Example ===\n";
    
    // The example should be run from the example file
    std::cout << "Executing pathway_generation.cpp example...\n" << std::endl;
    
    // Since main.cpp is the actual entry point, we're just forwarding to the example
    // This would normally be done via the command line or a separate executable
    extern int pathwayGenerationMain();
    pathwayGenerationMain();
}

