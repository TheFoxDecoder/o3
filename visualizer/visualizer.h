/**
 * @file visualizer.h
 * @brief Definition of the visualizer for the Ozone (O3) architecture.
 * 
 * This file contains the Visualizer class which provides a basic
 * text-based visualization of the network structure, neuron states,
 * and connections. In a full implementation, this would be a graphical
 * visualization, but for simplicity, we're using text-based output.
 */

#ifndef VISUALIZER_H
#define VISUALIZER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include "../include/neuron.h"
#include "../include/network.h"

/**
 * @brief Class for visualizing the neuromorphic network
 */
class Visualizer {
public:
    /**
     * @brief Struct representing a neuron in the visualization
     */
    struct VisualNeuron {
        std::string id;
        float x;                 // X position (0-1)
        float y;                 // Y position (0-1)
        int radius;              // Display radius
        std::string color;       // Color name or hex code
        std::string label;       // Short text label
        bool highlighted;        // Whether the neuron is highlighted
        Neuron::NeuronType type; // Type of neuron
        Neuron::NeuronState state; // Current state
    };
    
    /**
     * @brief Struct representing a connection in the visualization
     */
    struct VisualConnection {
        std::string sourceId;    // ID of source neuron
        std::string targetId;    // ID of target neuron
        float weight;            // Connection weight
        std::string color;       // Color name or hex code
        bool active;             // Whether the connection is currently active
    };
    
    /**
     * @brief Constructor for basic visualization
     * @param width Width of the visualization
     * @param height Height of the visualization
     */
    Visualizer(int width, int height);
    
    /**
     * @brief Constructor with network
     * @param network The network to visualize
     */
    Visualizer(Network& network);
    
    /**
     * @brief Add a neuron to the visualization
     * @param neuron The neuron to add
     * @param x X position (0-1)
     * @param y Y position (0-1)
     * @param radius Display radius
     * @param color Color for display
     */
    void addNeuron(std::shared_ptr<Neuron> neuron, float x, float y, 
                  int radius = 10, const std::string& color = "white");
    
    /**
     * @brief Add a connection to the visualization
     * @param sourceId ID of source neuron
     * @param targetId ID of target neuron
     * @param weight Connection weight
     * @param color Color for display
     */
    void addConnection(const std::string& sourceId, const std::string& targetId,
                      float weight = 1.0f, const std::string& color = "white");
    
    /**
     * @brief Update the visualization
     * Updates the display based on current neuron states
     */
    void update();
    
    /**
     * @brief Set neuron position
     * @param id ID of the neuron
     * @param x New X position
     * @param y New Y position
     */
    void setNeuronPosition(const std::string& id, float x, float y);
    
    /**
     * @brief Set neuron color
     * @param id ID of the neuron
     * @param color New color
     */
    void setNeuronColor(const std::string& id, const std::string& color);
    
    /**
     * @brief Highlight a specific neuron
     * @param id ID of the neuron
     * @param highlighted Whether to highlight
     */
    void highlightNeuron(const std::string& id, bool highlighted = true);
    
    /**
     * @brief Show the visualization
     * Displays the current state of the network
     */
    void show() const;
    
    /**
     * @brief Generate a network diagram
     * @return String representation of the network
     */
    std::string generateDiagram() const;
    
private:
    int width;                                            // Width of the visualization
    int height;                                           // Height of the visualization
    std::unordered_map<std::string, VisualNeuron> neurons;        // Map of neurons by ID
    std::vector<VisualConnection> connections;                    // List of connections
    
    Network* network = nullptr;                         // Reference to the network (if provided)
    
    mutable std::mutex mutex;                           // Synchronization
    
    /**
     * @brief Update the visual state from the actual neuron
     * @param neuronId ID of the neuron to update
     */
    void updateNeuronState(const std::string& neuronId);
    
    /**
     * @brief Update all connection activity states
     */
    void updateConnectionStates();
    
    /**
     * @brief Layout the neurons automatically
     * Arranges neurons in a reasonable layout
     */
    void autoLayout();
    
    /**
     * @brief Show information about the network
     */
    void showNetwork() const;
    
    /**
     * @brief Calculate color based on neuron state
     * @param state The neuron state
     * @return Color string
     */
    std::string stateToColor(Neuron::NeuronState state) const;
};

#endif // VISUALIZER_H

