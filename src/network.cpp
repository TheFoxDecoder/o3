/**
 * @file network.cpp
 * @brief Implementation of the Network classes for the Ozone (O3) architecture.
 */

#include "../include/network.h"
#include <algorithm>
#include <sstream>
#include <iostream>

// ============== Base Network Implementation ==============

Network::Network(const std::string& id) : id(id), processing(false) {
}

Network::~Network() {
    // Ensure processing is stopped
    processing = false;
    
    // Clear all connections between neurons
    for (auto& [_, neuron] : neurons) {
        auto outputs = neuron->getOutputs();
        for (auto& output : outputs) {
            neuron->disconnectFrom(output);
        }
    }
    
    // Clear all collections
    inputNeurons.clear();
    outputNeurons.clear();
    neurons.clear();
}

const std::string& Network::getId() const {
    return id;
}

std::shared_ptr<Neuron> Network::createNeuron(const std::string& id, Neuron::NeuronType type) {
    std::lock_guard<std::mutex> lock(neuronMutex);
    
    // Check if a neuron with this ID already exists
    if (neurons.find(id) != neurons.end()) {
        return neurons[id];  // Return existing neuron
    }
    
    // Create a new neuron
    auto neuron = std::make_shared<Neuron>(id, type);
    neurons[id] = neuron;
    
    return neuron;
}

bool Network::addNeuron(std::shared_ptr<Neuron> neuron) {
    if (!neuron) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(neuronMutex);
    
    // Check if a neuron with this ID already exists
    if (neurons.find(neuron->getId()) != neurons.end()) {
        return false;  // Already exists
    }
    
    neurons[neuron->getId()] = neuron;
    return true;
}

std::shared_ptr<Neuron> Network::getNeuron(const std::string& id) const {
    std::lock_guard<std::mutex> lock(neuronMutex);
    
    auto it = neurons.find(id);
    if (it != neurons.end()) {
        return it->second;
    }
    
    return nullptr;  // Not found
}

bool Network::removeNeuron(const std::string& id) {
    std::lock_guard<std::mutex> lock(neuronMutex);
    
    auto it = neurons.find(id);
    if (it == neurons.end()) {
        return false;  // Not found
    }
    
    auto neuron = it->second;
    
    // Disconnect this neuron from all others
    auto outputs = neuron->getOutputs();
    for (auto& output : outputs) {
        neuron->disconnectFrom(output);
    }
    
    // Disconnect all inputs to this neuron
    auto inputs = neuron->getInputs();
    for (auto& input : inputs) {
        input->disconnectFrom(neuron);
    }
    
    // Remove from input/output collections if present
    inputNeurons.erase(
        std::remove(inputNeurons.begin(), inputNeurons.end(), neuron),
        inputNeurons.end()
    );
    
    outputNeurons.erase(
        std::remove(outputNeurons.begin(), outputNeurons.end(), neuron),
        outputNeurons.end()
    );
    
    // Remove from main collection
    neurons.erase(it);
    
    return true;
}

bool Network::connectNeurons(const std::string& sourceId, const std::string& targetId, float weight) {
    auto source = getNeuron(sourceId);
    auto target = getNeuron(targetId);
    
    if (!source || !target) {
        return false;  // One or both neurons not found
    }
    
    return source->connectTo(target, weight);
}

bool Network::disconnectNeurons(const std::string& sourceId, const std::string& targetId) {
    auto source = getNeuron(sourceId);
    auto target = getNeuron(targetId);
    
    if (!source || !target) {
        return false;  // One or both neurons not found
    }
    
    return source->disconnectFrom(target);
}

std::vector<std::shared_ptr<Neuron>> Network::getAllNeurons() const {
    std::lock_guard<std::mutex> lock(neuronMutex);
    
    std::vector<std::shared_ptr<Neuron>> result;
    result.reserve(neurons.size());
    
    for (const auto& [_, neuron] : neurons) {
        result.push_back(neuron);
    }
    
    return result;
}

std::vector<std::shared_ptr<Neuron>> Network::getNeuronsByType(Neuron::NeuronType type) const {
    std::lock_guard<std::mutex> lock(neuronMutex);
    
    std::vector<std::shared_ptr<Neuron>> result;
    
    for (const auto& [_, neuron] : neurons) {
        if (neuron->getType() == type) {
            result.push_back(neuron);
        }
    }
    
    return result;
}

std::vector<std::shared_ptr<Neuron>> Network::getNeuronsByTag(const std::string& tag) const {
    std::lock_guard<std::mutex> lock(neuronMutex);
    
    std::vector<std::shared_ptr<Neuron>> result;
    
    for (const auto& [_, neuron] : neurons) {
        if (neuron->hasTag(tag)) {
            result.push_back(neuron);
        }
    }
    
    return result;
}

void Network::processSignals() {
    if (processing.exchange(true)) {
        return;  // Already processing
    }
    
    // Process all neurons in the network
    auto allNeurons = getAllNeurons();
    
    // First process input neurons
    for (auto& neuron : inputNeurons) {
        neuron->processSignals();
    }
    
    // Then process all other neurons (excluding input and output)
    for (auto& neuron : allNeurons) {
        // Skip if this is an input or output neuron
        if (std::find(inputNeurons.begin(), inputNeurons.end(), neuron) != inputNeurons.end() ||
            std::find(outputNeurons.begin(), outputNeurons.end(), neuron) != outputNeurons.end()) {
            continue;
        }
        
        neuron->processSignals();
    }
    
    // Finally process output neurons
    for (auto& neuron : outputNeurons) {
        neuron->processSignals();
    }
    
    // Call process callbacks
    for (auto& callback : processCallbacks) {
        callback(*this);
    }
    
    processing = false;
}

void Network::reset() {
    std::lock_guard<std::mutex> lock(neuronMutex);
    
    for (auto& [_, neuron] : neurons) {
        neuron->setState(Neuron::NeuronState::RESTING);
    }
}

void Network::addInputNeuron(std::shared_ptr<Neuron> inputNeuron) {
    if (!inputNeuron) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(neuronMutex);
    
    // Check if already in the collection
    if (std::find(inputNeurons.begin(), inputNeurons.end(), inputNeuron) != inputNeurons.end()) {
        return;  // Already added
    }
    
    // Add to the network if not already there
    if (neurons.find(inputNeuron->getId()) == neurons.end()) {
        neurons[inputNeuron->getId()] = inputNeuron;
    }
    
    // Add to input collection
    inputNeurons.push_back(inputNeuron);
}

void Network::addOutputNeuron(std::shared_ptr<Neuron> outputNeuron) {
    if (!outputNeuron) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(neuronMutex);
    
    // Check if already in the collection
    if (std::find(outputNeurons.begin(), outputNeurons.end(), outputNeuron) != outputNeurons.end()) {
        return;  // Already added
    }
    
    // Add to the network if not already there
    if (neurons.find(outputNeuron->getId()) == neurons.end()) {
        neurons[outputNeuron->getId()] = outputNeuron;
    }
    
    // Add to output collection
    outputNeurons.push_back(outputNeuron);
}

std::vector<std::shared_ptr<Neuron>> Network::getInputNeurons() const {
    std::lock_guard<std::mutex> lock(neuronMutex);
    return inputNeurons;
}

std::vector<std::shared_ptr<Neuron>> Network::getOutputNeurons() const {
    std::lock_guard<std::mutex> lock(neuronMutex);
    return outputNeurons;
}

bool Network::injectSignal(std::shared_ptr<Synapse> signal, const std::string& targetId) {
    if (!signal) {
        return false;
    }
    
    if (!targetId.empty()) {
        // Send to specific neuron
        auto target = getNeuron(targetId);
        if (!target) {
            return false;  // Target not found
        }
        
        target->receiveSignal(signal);
        return true;
    }
    
    // Send to all input neurons
    bool delivered = false;
    
    for (auto& inputNeuron : getInputNeurons()) {
        inputNeuron->receiveSignal(signal);
        delivered = true;
    }
    
    return delivered;
}

void Network::onProcess(std::function<void(Network&)> callback) {
    if (callback) {
        processCallbacks.push_back(callback);
    }
}

std::string Network::visualize() const {
    std::stringstream ss;
    
    ss << "Network: " << id << std::endl;
    ss << "Neurons: " << getNeuronCount() << std::endl;
    ss << "Connections: " << getConnectionCount() << std::endl;
    
    ss << "\nInput Neurons: ";
    for (const auto& neuron : getInputNeurons()) {
        ss << neuron->getId() << " ";
    }
    
    ss << "\nOutput Neurons: ";
    for (const auto& neuron : getOutputNeurons()) {
        ss << neuron->getId() << " ";
    }
    
    ss << "\n\nConnections:\n";
    for (const auto& [_, neuron] : neurons) {
        auto outputs = neuron->getOutputs();
        if (!outputs.empty()) {
            ss << neuron->getId() << " -> ";
            for (const auto& output : outputs) {
                ss << output->getId() << "(" << neuron->getConnectionWeight(output) << ") ";
            }
            ss << std::endl;
        }
    }
    
    return ss.str();
}

size_t Network::getNeuronCount() const {
    std::lock_guard<std::mutex> lock(neuronMutex);
    return neurons.size();
}

size_t Network::getConnectionCount() const {
    std::lock_guard<std::mutex> lock(neuronMutex);
    
    size_t count = 0;
    
    for (const auto& [_, neuron] : neurons) {
        count += neuron->getOutputs().size();
    }
    
    return count;
}

bool Network::isProcessing() const {
    return processing;
}

// ============== Conscious Network Implementation ==============

ConsciousNetwork::ConsciousNetwork(const std::string& id)
    : Network(id), attentionStrength(0.5f) {
}

void ConsciousNetwork::setAttentionFocus(const std::string& neuronId) {
    focusedNeuronId = neuronId;
}

std::string ConsciousNetwork::getAttentionFocus() const {
    return focusedNeuronId;
}

void ConsciousNetwork::processSignals() {
    if (isProcessing()) {
        return;  // Already processing
    }
    
    // Special processing for conscious network
    
    // If there's a focused neuron, boost its activation
    if (!focusedNeuronId.empty()) {
        auto focusedNeuron = getNeuron(focusedNeuronId);
        if (focusedNeuron) {
            // Create an attention signal
            auto attentionSignal = std::make_shared<Synapse>("attention_signal");
            attentionSignal->setStrength(attentionStrength);
            attentionSignal->setData("type", std::string("attention"));
            attentionSignal->setData("source", std::string("conscious_control"));
            
            // Inject into the focused neuron
            focusedNeuron->receiveSignal(attentionSignal);
        }
    }
    
    // Proceed with normal processing
    Network::processSignals();
}

// ============== Subconscious Network Implementation ==============

SubconsciousNetwork::SubconsciousNetwork(const std::string& id)
    : Network(id) {
}

void SubconsciousNetwork::addPattern(const std::vector<std::string>& pattern, 
                                    const std::vector<std::string>& response) {
    patterns.push_back(std::make_pair(pattern, response));
}

void SubconsciousNetwork::processSignals() {
    if (isProcessing()) {
        return;  // Already processing
    }
    
    // Check for pattern matches before normal processing
    for (const auto& [pattern, response] : patterns) {
        if (matchesPattern(pattern)) {
            generateResponse(response);
        }
    }
    
    // Proceed with normal processing
    Network::processSignals();
}

bool SubconsciousNetwork::matchesPattern(const std::vector<std::string>& pattern) const {
    // Simplified pattern matching
    // A real implementation would be more sophisticated
    
    // For this example, we'll check if any neuron has metadata matching each pattern item
    for (const auto& patternItem : pattern) {
        bool found = false;
        
        for (const auto& [_, neuron] : neurons) {
            if (neuron->hasMetadata(patternItem) || neuron->hasTag(patternItem)) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            return false;  // Pattern item not found
        }
    }
    
    return true;  // All pattern items found
}

void SubconsciousNetwork::generateResponse(const std::vector<std::string>& response) {
    // Simplified response generation
    // In a real implementation, this would create more complex neural activations
    
    for (const auto& outputNeuron : getOutputNeurons()) {
        // Create a response signal
        auto responseSignal = std::make_shared<Synapse>("response_signal");
        responseSignal->setStrength(0.8f);
        
        // Add response data
        for (size_t i = 0; i < response.size(); ++i) {
            responseSignal->setData("response_" + std::to_string(i), response[i]);
        }
        
        // Send to output neuron
        outputNeuron->receiveSignal(responseSignal);
    }
}

// ============== Unconscious Network Implementation ==============

UnconsciousNetwork::UnconsciousNetwork(const std::string& id)
    : Network(id) {
}

void UnconsciousNetwork::addFilterRule(const std::string& key, const std::string& value) {
    filterRules.push_back(std::make_pair(key, value));
}

void UnconsciousNetwork::processSignals() {
    if (isProcessing()) {
        return;  // Already processing
    }
    
    // In unconscious processing, we filter signals before processing
    
    // Collect all signals from input neurons
    std::vector<std::shared_ptr<Synapse>> inputSignals;
    
    // Process filtered signals
    Network::processSignals();
}

bool UnconsciousNetwork::passesFilters(const std::shared_ptr<Synapse>& signal) const {
    if (!signal) {
        return false;
    }
    
    // If no filter rules, everything passes
    if (filterRules.empty()) {
        return true;
    }
    
    // Check against each filter rule
    for (const auto& [key, value] : filterRules) {
        if (signal->hasData(key)) {
            std::string signalValue = signal->getData<std::string>(key);
            if (signalValue == value) {
                return true;  // Matches a filter rule
            }
        }
    }
    
    return false;  // Doesn't match any filter rule
}

// ============== Network Factory Implementation ==============

std::shared_ptr<Network> NetworkFactory::createNetwork(NetworkType type, const std::string& id) {
    switch (type) {
        case NetworkType::CONSCIOUS:
            return std::make_shared<ConsciousNetwork>(id);
            
        case NetworkType::SUBCONSCIOUS:
            return std::make_shared<SubconsciousNetwork>(id);
            
        case NetworkType::UNCONSCIOUS:
            return std::make_shared<UnconsciousNetwork>(id);
            
        case NetworkType::BASIC:
        default:
            return std::make_shared<Network>(id);
    }
}

