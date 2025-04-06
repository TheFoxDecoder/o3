/**
 * @file visualizer.cpp
 * @brief Implementation of the Visualizer class for the Ozone (O3) architecture.
 */

#include "../visualizer/visualizer.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cmath>

Visualizer::Visualizer(int width, int height) : width(width), height(height) {
}

Visualizer::Visualizer(Network& network) : width(800), height(600), network(&network) {
    // Initialize with neurons from the network
    auto allNeurons = network.getAllNeurons();
    
    // Auto-layout neurons
    int totalNeurons = allNeurons.size();
    if (totalNeurons == 0) return;
    
    // Simple circular layout for now
    for (size_t i = 0; i < allNeurons.size(); ++i) {
        auto neuron = allNeurons[i];
        
        // Calculate position on a circle
        float angle = (2.0f * M_PI * i) / totalNeurons;
        float x = 0.5f + 0.4f * std::cos(angle);  // Centered with 0.4 radius
        float y = 0.5f + 0.4f * std::sin(angle);  // Centered with 0.4 radius
        
        // Determine color based on neuron type
        std::string color;
        switch (neuron->getType()) {
            case Neuron::NeuronType::SENSORY:
                color = "blue";
                break;
            case Neuron::NeuronType::OUTPUT:
                color = "green";
                break;
            case Neuron::NeuronType::MEMORY:
                color = "purple";
                break;
            case Neuron::NeuronType::REGULATORY:
                color = "red";
                break;
            default:
                color = "white";
                break;
        }
        
        // Add to visualization
        addNeuron(neuron, x, y, 10, color);
    }
    
    // Add connections
    for (auto& neuron : allNeurons) {
        auto outputs = neuron->getOutputs();
        for (auto& output : outputs) {
            addConnection(neuron->getId(), output->getId(), 
                          neuron->getConnectionWeight(output));
        }
    }
}

void Visualizer::addNeuron(std::shared_ptr<Neuron> neuron, float x, float y, 
                         int radius, const std::string& color) {
    if (!neuron) return;
    
    std::lock_guard<std::mutex> lock(mutex);
    
    VisualNeuron vn;
    vn.id = neuron->getId();
    vn.x = x;
    vn.y = y;
    vn.radius = radius;
    vn.color = color;
    vn.label = neuron->getId();
    vn.highlighted = false;
    vn.type = neuron->getType();
    vn.state = neuron->getState();
    
    neurons[vn.id] = vn;
}

void Visualizer::addConnection(const std::string& sourceId, const std::string& targetId,
                             float weight, const std::string& color) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Ensure both neurons exist
    if (neurons.find(sourceId) == neurons.end() || 
        neurons.find(targetId) == neurons.end()) {
        return;
    }
    
    VisualConnection vc;
    vc.sourceId = sourceId;
    vc.targetId = targetId;
    vc.weight = weight;
    vc.color = color;
    vc.active = false;
    
    connections.push_back(vc);
}

void Visualizer::update() {
    if (!network) return;
    
    std::lock_guard<std::mutex> lock(mutex);
    
    // Update each neuron
    for (auto& [id, _] : neurons) {
        updateNeuronState(id);
    }
    
    // Update connections
    updateConnectionStates();
}

void Visualizer::setNeuronPosition(const std::string& id, float x, float y) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = neurons.find(id);
    if (it != neurons.end()) {
        it->second.x = x;
        it->second.y = y;
    }
}

void Visualizer::setNeuronColor(const std::string& id, const std::string& color) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = neurons.find(id);
    if (it != neurons.end()) {
        it->second.color = color;
    }
}

void Visualizer::highlightNeuron(const std::string& id, bool highlighted) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = neurons.find(id);
    if (it != neurons.end()) {
        it->second.highlighted = highlighted;
    }
}

void Visualizer::show() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    // Show basic network info if available
    if (network) {
        showNetwork();
    }
    
    // Generate and display an ASCII diagram
    std::cout << generateDiagram() << std::endl;
}

std::string Visualizer::generateDiagram() const {
    // Create a simple ASCII diagram
    std::stringstream ss;
    
    // Count neurons by type for a summary
    std::unordered_map<Neuron::NeuronType, int> typeCounts;
    for (const auto& [id, neuron] : neurons) {
        typeCounts[neuron.type]++;
    }
    
    // Display summary
    ss << "+----------------------------------+" << std::endl;
    ss << "| Network Visualization            |" << std::endl;
    ss << "+----------------------------------+" << std::endl;
    ss << "| Total Neurons: " << std::setw(18) << neurons.size() << " |" << std::endl;
    ss << "| Total Connections: " << std::setw(14) << connections.size() << " |" << std::endl;
    ss << "+----------------------------------+" << std::endl;
    ss << "| Neuron Types:                    |" << std::endl;
    
    // Display neuron type counts
    for (const auto& [type, count] : typeCounts) {
        ss << "| - ";
        switch (type) {
            case Neuron::NeuronType::SENSORY:
                ss << "Sensory: ";
                break;
            case Neuron::NeuronType::PROCESSING:
                ss << "Processing: ";
                break;
            case Neuron::NeuronType::MEMORY:
                ss << "Memory: ";
                break;
            case Neuron::NeuronType::INTEGRATION:
                ss << "Integration: ";
                break;
            case Neuron::NeuronType::ASSOCIATION:
                ss << "Association: ";
                break;
            case Neuron::NeuronType::OUTPUT:
                ss << "Output: ";
                break;
            case Neuron::NeuronType::REGULATORY:
                ss << "Regulatory: ";
                break;
            default:
                ss << "Unknown: ";
                break;
        }
        ss << std::setw(20 - 11) << count << " |" << std::endl;  // 11 is max type name length
    }
    
    ss << "+----------------------------------+" << std::endl;
    
    // In a full implementation, we would generate a more sophisticated
    // visual representation here. For now, just list active neurons.
    ss << "| Active Neurons:                  |" << std::endl;
    bool hasActive = false;
    
    for (const auto& [id, neuron] : neurons) {
        if (neuron.state == Neuron::NeuronState::ACTIVE) {
            ss << "| - " << std::left << std::setw(30) << neuron.id << " |" << std::endl;
            hasActive = true;
        }
    }
    
    if (!hasActive) {
        ss << "| (None)                           |" << std::endl;
    }
    
    ss << "+----------------------------------+" << std::endl;
    
    return ss.str();
}

void Visualizer::updateNeuronState(const std::string& neuronId) {
    if (!network) return;
    
    auto neuron = network->getNeuron(neuronId);
    if (!neuron) return;
    
    auto it = neurons.find(neuronId);
    if (it != neurons.end()) {
        it->second.state = neuron->getState();
        
        // Update color based on state
        if (!it->second.highlighted) {
            it->second.color = stateToColor(neuron->getState());
        }
    }
}

void Visualizer::updateConnectionStates() {
    if (!network) return;
    
    // In a full implementation, this would track signal flow
    // For simplicity, we'll just mark connections as active if
    // the source neuron is in an active state
    
    for (auto& conn : connections) {
        auto sourceIt = neurons.find(conn.sourceId);
        
        if (sourceIt != neurons.end() && 
            sourceIt->second.state == Neuron::NeuronState::ACTIVE) {
            conn.active = true;
            conn.color = "yellow";  // Highlight active connections
        } else {
            conn.active = false;
            conn.color = "white";   // Reset inactive connections
        }
    }
}

void Visualizer::autoLayout() {
    // Simple circular layout
    int totalNeurons = neurons.size();
    if (totalNeurons == 0) return;
    
    // Arrange neurons in a circle
    int i = 0;
    for (auto& [id, neuron] : neurons) {
        // Calculate position on a circle
        float angle = (2.0f * M_PI * i) / totalNeurons;
        neuron.x = 0.5f + 0.4f * std::cos(angle);  // Centered with 0.4 radius
        neuron.y = 0.5f + 0.4f * std::sin(angle);  // Centered with 0.4 radius
        
        i++;
    }
}

void Visualizer::showNetwork() const {
    std::cout << "Network: " << network->getId() << std::endl;
    std::cout << "  Neurons: " << network->getNeuronCount() << std::endl;
    std::cout << "  Connections: " << network->getConnectionCount() << std::endl;
    
    // Display input neurons
    auto inputs = network->getInputNeurons();
    std::cout << "  Input Neurons: ";
    for (const auto& input : inputs) {
        std::cout << input->getId() << " ";
    }
    std::cout << std::endl;
    
    // Display output neurons
    auto outputs = network->getOutputNeurons();
    std::cout << "  Output Neurons: ";
    for (const auto& output : outputs) {
        std::cout << output->getId() << " ";
    }
    std::cout << std::endl;
}

std::string Visualizer::stateToColor(Neuron::NeuronState state) const {
    switch (state) {
        case Neuron::NeuronState::ACTIVE:
            return "green";
        case Neuron::NeuronState::INHIBITED:
            return "red";
        case Neuron::NeuronState::REFRACTORY:
            return "orange";
        case Neuron::NeuronState::RESTING:
        default:
            return "white";
    }
}

