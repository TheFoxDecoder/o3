/**
 * @file neuron.cpp
 * @brief Implementation of the Neuron class.
 * 
 * This file contains the implementation of the Neuron class 
 * that simulates biological neuron behavior.
 */

#include <algorithm>
#include <cmath>
#include <iostream>
#include <random>
#include <sstream>
#include "../include/neuron.h"
#include "../include/utils.h"

Neuron::Neuron(const std::string& id, NeuronType type) : 
    id(id), 
    type(type),
    state(NeuronState::RESTING),
    threshold(0.5f),
    potential(0.0f),
    refractoryPeriod(false) {
        
    // Initialize neuron parameters based on type
    switch (type) {
        case NeuronType::SENSORY:
            threshold = 0.3f;  // Lower threshold for sensory neurons
            break;
            
        case NeuronType::MEMORY:
            threshold = 0.7f;  // Higher threshold for memory neurons
            break;
            
        case NeuronType::REGULATORY:
            threshold = 0.4f;  // Moderate threshold for regulatory neurons
            break;
            
        default:
            threshold = 0.5f;  // Default threshold
            break;
    }
    
    // Add type tag
    std::string typeTag;
    switch (type) {
        case NeuronType::SENSORY: typeTag = "sensory"; break;
        case NeuronType::PROCESSING: typeTag = "processing"; break;
        case NeuronType::MEMORY: typeTag = "memory"; break;
        case NeuronType::INTEGRATION: typeTag = "integration"; break;
        case NeuronType::ASSOCIATION: typeTag = "association"; break;
        case NeuronType::OUTPUT: typeTag = "output"; break;
        case NeuronType::REGULATORY: typeTag = "regulatory"; break;
    }
    
    addTag(typeTag);
}

std::shared_ptr<NeuronGate> Neuron::createGate(NeuronGate::GateType gateType) {
    std::string gateId = id + "_gate_" + std::to_string(gates.size());
    auto gate = NeuronGateFactory::createGate(gateType, gateId);
    
    if (gate) {
        gates.push_back(gate);
    }
    
    return gate;
}

void Neuron::setState(NeuronState state) {
    // Store old state for callbacks
    NeuronState oldState = this->state;
    
    // Update state
    this->state = state;
    
    // Call state change callbacks
    for (const auto& callback : stateChangeCallbacks) {
        callback(shared_from_this(), oldState, state);
    }
    
    // If becoming active, allocate memory
    if (state == NeuronState::ACTIVE && oldState != NeuronState::ACTIVE) {
        // This would involve more memory management in a real implementation
    }
    
    // If becoming inactive, deallocate memory
    if (state == NeuronState::RESTING && oldState != NeuronState::RESTING) {
        // This would involve more memory management in a real implementation
    }
}

Neuron::NeuronState Neuron::getState() const {
    return state;
}

const std::string& Neuron::getId() const {
    return id;
}

Neuron::NeuronType Neuron::getType() const {
    return type;
}

void Neuron::setThreshold(float threshold) {
    if (threshold < 0.0f) threshold = 0.0f;
    if (threshold > 1.0f) threshold = 1.0f;
    this->threshold = threshold;
}

float Neuron::getThreshold() const {
    return threshold;
}

bool Neuron::connectTo(std::shared_ptr<Neuron> target, float weight) {
    if (!target || target.get() == this) {
        return false;  // Can't connect to null or self
    }
    
    // Check if already connected
    if (connections.find(target) != connections.end()) {
        // Update weight
        connections[target] = weight;
        return true;
    }
    
    // Add new connection
    connections[target] = weight;
    
    // Add this neuron as an input to the target
    target->inputs.push_back(weak_from_this());
    
    return true;
}

bool Neuron::disconnectFrom(std::shared_ptr<Neuron> target) {
    if (!target) {
        return false;
    }
    
    // Check if connected
    auto it = connections.find(target);
    if (it == connections.end()) {
        return false;  // Not connected
    }
    
    // Remove connection
    connections.erase(it);
    
    // Remove this neuron from target's inputs
    auto& targetInputs = target->inputs;
    targetInputs.erase(
        std::remove_if(targetInputs.begin(), targetInputs.end(),
                      [this](const std::weak_ptr<Neuron>& weakPtr) {
                          auto ptr = weakPtr.lock();
                          return !ptr || ptr.get() == this;
                      }),
        targetInputs.end()
    );
    
    return true;
}

void Neuron::receiveSignal(std::shared_ptr<Synapse> signal) {
    if (!signal) {
        return;
    }
    
    // Add to input signals
    inputSignals.push_back(signal);
    
    // Note: In a more complex implementation, we might queue signals
    // based on timing, priority, etc.
    
    // Process signals immediately or queue for later processing
    // depending on the architecture configuration
    processSignals();
}

void Neuron::processSignals() {
    if (state == NeuronState::REFRACTORY || state == NeuronState::INHIBITED) {
        return;  // Can't process signals in these states
    }
    
    if (inputSignals.empty()) {
        return;  // No signals to process
    }
    
    // Apply gates to the input signals based on neuron type
    std::vector<std::shared_ptr<Synapse>> processed;
    
    for (const auto& signal : inputSignals) {
        // Process through appropriate gates based on neuron type and signal tags
        bool handled = false;
        
        // Find a gate that can handle this signal
        for (const auto& gate : gates) {
            if (gate && gate->isActive()) {
                std::vector<std::shared_ptr<Synapse>> gateInput = { signal };
                auto result = gate->process(gateInput);
                
                if (result) {
                    processed.push_back(result);
                    handled = true;
                    break;  // Signal handled by this gate
                }
            }
        }
        
        // If no gate handled it, pass through as-is
        if (!handled && signal) {
            processed.push_back(signal);
        }
    }
    
    // Clear input signals
    inputSignals.clear();
    
    // Calculate contribution to potential
    float potentialDelta = 0.0f;
    
    for (const auto& signal : processed) {
        // Get signal strength
        float strength = 0.5f;  // Default strength
        
        try {
            if (signal->hasData("strength")) {
                strength = std::stof(signal->getData<std::string>("strength"));
            }
        } catch (...) {
            // Use default
        }
        
        // Accumulate potential
        potentialDelta += strength;
    }
    
    // Update potential
    potential += potentialDelta / (processed.size() > 0 ? processed.size() : 1.0f);
    
    // Ensure potential is within bounds
    if (potential < 0.0f) potential = 0.0f;
    if (potential > 1.0f) potential = 1.0f;
    
    // Check if potential exceeds threshold
    if (potential >= threshold) {
        // Fire the neuron
        fire();
        
        // Enter refractory period (for biological realism)
        setState(NeuronState::REFRACTORY);
        
        // Reset potential
        potential = 0.0f;
        
        // Schedule transition back to resting state
        // (In a real implementation, this would be time-based)
        setState(NeuronState::RESTING);
    }
    
    // Store processed signals for output or memory
    for (const auto& signal : processed) {
        outputSignals.push_back(signal);
    }
}

void Neuron::fire() {
    if (outputSignals.empty()) {
        // Create a default output signal if none exists
        auto signal = std::make_shared<Synapse>(id + "_output");
        signal->setData("source", id);
        signal->setData("strength", std::to_string(potential));
        outputSignals.push_back(signal);
    }
    
    // Send signals to connected neurons
    for (const auto& [target, weight] : connections) {
        if (target) {
            for (const auto& signal : outputSignals) {
                // Create a weighted copy of the signal
                auto weighted = signal->derive();
                
                // Apply weight to strength
                float strength = 0.5f;
                try {
                    if (signal->hasData("strength")) {
                        strength = std::stof(signal->getData<std::string>("strength"));
                    }
                } catch (...) {
                    // Use default
                }
                
                // Apply connection weight
                strength *= weight;
                
                // Set new strength
                weighted->setData("strength", std::to_string(strength));
                
                // Add connection metadata
                weighted->setData("from", id);
                weighted->setData("to", target->getId());
                
                // Send to target
                target->receiveSignal(weighted);
            }
        }
    }
    
    // Call fire callbacks
    for (const auto& callback : fireCallbacks) {
        callback(shared_from_this());
    }
}

void Neuron::addTag(const std::string& tag) {
    // Check if tag already exists
    if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
        tags.push_back(tag);
    }
}

bool Neuron::hasTag(const std::string& tag) const {
    return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

std::vector<std::string> Neuron::getTags() const {
    return tags;
}

void Neuron::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string Neuron::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    if (it != metadata.end()) {
        return it->second;
    }
    return "";
}

bool Neuron::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

float Neuron::getPotential() const {
    return potential;
}

std::vector<std::shared_ptr<Neuron>> Neuron::getInputs() const {
    std::vector<std::shared_ptr<Neuron>> result;
    
    for (const auto& weakInput : inputs) {
        if (auto input = weakInput.lock()) {
            result.push_back(input);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<Neuron>> Neuron::getOutputs() const {
    std::vector<std::shared_ptr<Neuron>> result;
    
    for (const auto& [target, _] : connections) {
        result.push_back(target);
    }
    
    return result;
}

void Neuron::onFire(std::function<void(std::shared_ptr<Neuron>)> callback) {
    if (callback) {
        fireCallbacks.push_back(callback);
    }
}

void Neuron::onStateChange(std::function<void(std::shared_ptr<Neuron>, NeuronState, NeuronState)> callback) {
    if (callback) {
        stateChangeCallbacks.push_back(callback);
    }
}

float Neuron::getConnectionWeight(std::shared_ptr<Neuron> target) const {
    auto it = connections.find(target);
    if (it != connections.end()) {
        return it->second;
    }
    return 0.0f;
}

bool Neuron::setConnectionWeight(std::shared_ptr<Neuron> target, float weight) {
    auto it = connections.find(target);
    if (it != connections.end()) {
        it->second = weight;
        return true;
    }
    return false;
}

void Neuron::reset() {
    potential = 0.0f;
    setState(NeuronState::RESTING);
    inputSignals.clear();
    outputSignals.clear();
}

bool Neuron::integrate() {
    // Example integration function (simplified)
    // In a real implementation, this would involve more complex
    // calculations based on signal timing, weights, etc.
    
    if (inputSignals.empty()) {
        return false;
    }
    
    // Create a default integration gate if none exists
    if (gates.empty()) {
        auto gate = createGate(NeuronGate::GateType::THRESHOLD);
        gate->setThreshold(threshold);
    }
    
    return potential >= threshold;
}

Neuron::~Neuron() {
    // Clean up connections
    connections.clear();
    inputs.clear();
    gates.clear();
    
    // Clear signals
    inputSignals.clear();
    outputSignals.clear();
    
    // Clear callbacks
    fireCallbacks.clear();
    stateChangeCallbacks.clear();
}

