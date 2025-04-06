/**
 * @file network.h
 * @brief Definition of network classes for the Ozone (O3) architecture.
 * 
 * This file contains the network classes that implement the three-tiered
 * system (Conscious, Subconscious, Unconscious) described in the Ozone (O3)
 * architecture paper.
 */

#ifndef NETWORK_H
#define NETWORK_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include "neuron.h"
#include "synapse.h"

/**
 * @brief Base class for all networks in the O3 architecture
 */
class Network {
public:
    /**
     * @brief Constructor for Network
     * @param id Unique identifier for this network
     */
    Network(const std::string& id);
    
    /**
     * @brief Destructor for Network
     */
    virtual ~Network();
    
    /**
     * @brief Get the network's unique identifier
     * @return The network ID
     */
    const std::string& getId() const;
    
    /**
     * @brief Create a new neuron in the network
     * @param id Unique identifier for the neuron
     * @param type Type of neuron to create
     * @return Shared pointer to the created neuron
     */
    std::shared_ptr<Neuron> createNeuron(const std::string& id, Neuron::NeuronType type);
    
    /**
     * @brief Add an existing neuron to the network
     * @param neuron Neuron to add
     * @return True if added successfully
     */
    bool addNeuron(std::shared_ptr<Neuron> neuron);
    
    /**
     * @brief Get a neuron by ID
     * @param id The ID of the neuron to retrieve
     * @return Shared pointer to the neuron or nullptr if not found
     */
    std::shared_ptr<Neuron> getNeuron(const std::string& id) const;
    
    /**
     * @brief Remove a neuron from the network
     * @param id ID of the neuron to remove
     * @return True if removed successfully
     */
    bool removeNeuron(const std::string& id);
    
    /**
     * @brief Connect two neurons in the network
     * @param sourceId ID of the source neuron
     * @param targetId ID of the target neuron
     * @param weight Connection weight
     * @return True if connection was successful
     */
    bool connectNeurons(const std::string& sourceId, const std::string& targetId, float weight = 1.0f);
    
    /**
     * @brief Disconnect two neurons in the network
     * @param sourceId ID of the source neuron
     * @param targetId ID of the target neuron
     * @return True if disconnection was successful
     */
    bool disconnectNeurons(const std::string& sourceId, const std::string& targetId);
    
    /**
     * @brief Get all neurons in the network
     * @return Vector of all neurons
     */
    std::vector<std::shared_ptr<Neuron>> getAllNeurons() const;
    
    /**
     * @brief Get neurons by type
     * @param type Type of neurons to retrieve
     * @return Vector of neurons matching the type
     */
    std::vector<std::shared_ptr<Neuron>> getNeuronsByType(Neuron::NeuronType type) const;
    
    /**
     * @brief Get neurons by tag
     * @param tag Tag to search for
     * @return Vector of neurons with the matching tag
     */
    std::vector<std::shared_ptr<Neuron>> getNeuronsByTag(const std::string& tag) const;
    
    /**
     * @brief Process signals through the network
     * This method propagates signals through all neurons in the network
     */
    void processSignals();
    
    /**
     * @brief Reset all neurons in the network to their initial state
     */
    void reset();
    
    /**
     * @brief Add a neuron to the input layer
     * @param inputNeuron Neuron to add as input
     */
    void addInputNeuron(std::shared_ptr<Neuron> inputNeuron);
    
    /**
     * @brief Add a neuron to the output layer
     * @param outputNeuron Neuron to add as output
     */
    void addOutputNeuron(std::shared_ptr<Neuron> outputNeuron);
    
    /**
     * @brief Get all input neurons
     * @return Vector of input neurons
     */
    std::vector<std::shared_ptr<Neuron>> getInputNeurons() const;
    
    /**
     * @brief Get all output neurons
     * @return Vector of output neurons
     */
    std::vector<std::shared_ptr<Neuron>> getOutputNeurons() const;
    
    /**
     * @brief Inject a signal into the network
     * @param signal The signal to inject
     * @param targetId The ID of the target neuron (or empty for all input neurons)
     * @return True if the signal was delivered
     */
    bool injectSignal(std::shared_ptr<Synapse> signal, const std::string& targetId = "");
    
    /**
     * @brief Register a callback for network events
     * @param callback Function to call when the network processes signals
     */
    void onProcess(std::function<void(Network&)> callback);
    
    /**
     * @brief Generate a visual representation of the network
     * @return String representing the network structure
     */
    std::string visualize() const;
    
    /**
     * @brief Get the number of neurons in the network
     * @return Count of neurons
     */
    size_t getNeuronCount() const;
    
    /**
     * @brief Get the number of connections in the network
     * @return Count of connections between neurons
     */
    size_t getConnectionCount() const;
    
    /**
     * @brief Get the network's processing state
     * @return True if the network is currently processing signals
     */
    bool isProcessing() const;
    
private:
    std::string id;  // Unique identifier
    
    // Neuron storage
    std::unordered_map<std::string, std::shared_ptr<Neuron>> neurons;
    
    // Input and output layers
    std::vector<std::shared_ptr<Neuron>> inputNeurons;
    std::vector<std::shared_ptr<Neuron>> outputNeurons;
    
    // Processing state
    std::atomic<bool> processing;
    
    // Callbacks
    std::vector<std::function<void(Network&)>> processCallbacks;
    
    // Thread synchronization
    mutable std::mutex neuronMutex;
};

/**
 * @brief Specialized Conscious tier network
 * 
 * The Conscious network handles high-level cognitive functions
 * and is aware of its own processing.
 */
class ConsciousNetwork : public Network {
public:
    /**
     * @brief Constructor for ConsciousNetwork
     * @param id Unique identifier
     */
    ConsciousNetwork(const std::string& id);
    
    /**
     * @brief Set the attention focus of the network
     * @param neuronId ID of the neuron to focus on
     */
    void setAttentionFocus(const std::string& neuronId);
    
    /**
     * @brief Get the current attention focus
     * @return ID of the neuron currently in focus
     */
    std::string getAttentionFocus() const;
    
    /**
     * @brief Process signals with attention bias
     * This overrides the base process method to include attention mechanisms
     */
    virtual void processSignals();
    
private:
    std::string focusedNeuronId;  // ID of the neuron currently in focus
    float attentionStrength;      // Strength of attention focus
};

/**
 * @brief Specialized Subconscious tier network
 * 
 * The Subconscious network handles pattern recognition and
 * routine processing without conscious awareness.
 */
class SubconsciousNetwork : public Network {
public:
    /**
     * @brief Constructor for SubconsciousNetwork
     * @param id Unique identifier
     */
    SubconsciousNetwork(const std::string& id);
    
    /**
     * @brief Add a pattern for recognition
     * @param pattern The pattern to recognize
     * @param response The response to generate when pattern is detected
     */
    void addPattern(const std::vector<std::string>& pattern, 
                   const std::vector<std::string>& response);
    
    /**
     * @brief Process signals with pattern recognition
     * This overrides the base process method to include pattern recognition
     */
    virtual void processSignals() ;
    
private:
    // Patterns stored as sequences of keys/values to match
    std::vector<std::pair<std::vector<std::string>, std::vector<std::string>>> patterns;
    
    /**
     * @brief Check if a pattern matches current network state
     * @param pattern The pattern to check
     * @return True if the pattern matches
     */
    bool matchesPattern(const std::vector<std::string>& pattern) const;
    
    /**
     * @brief Generate a response for a matched pattern
     * @param response The response to generate
     */
    void generateResponse(const std::vector<std::string>& response);
};

/**
 * @brief Specialized Unconscious tier network
 * 
 * The Unconscious network handles basic survival functions,
 * reflexes, and deep memory.
 */
class UnconsciousNetwork : public Network {
public:
    /**
     * @brief Constructor for UnconsciousNetwork
     * @param id Unique identifier
     */
    UnconsciousNetwork(const std::string& id);
    
    /**
     * @brief Add a rule for filtering signals
     * @param key The metadata key to filter on
     * @param value The value to filter for
     */
    void addFilterRule(const std::string& key, const std::string& value);
    
    /**
     * @brief Process signals with filtering
     * This overrides the base process method to include signal filtering
     */
    virtual void processSignals() ;
    
private:
    // Signal filtering rules
    std::vector<std::pair<std::string, std::string>> filterRules;
    
    /**
     * @brief Check if a signal passes the filter rules
     * @param signal The signal to check
     * @return True if the signal passes all filters
     */
    bool passesFilters(const std::shared_ptr<Synapse>& signal) const;
};

/**
 * @brief Factory for creating different types of networks
 */
class NetworkFactory {
public:
    /**
     * @brief Network types available for creation
     */
    enum class NetworkType {
        BASIC,          // Standard network
        CONSCIOUS,      // Conscious tier
        SUBCONSCIOUS,   // Subconscious tier
        UNCONSCIOUS     // Unconscious tier
    };
    
    /**
     * @brief Create a network of the specified type
     * @param type Type of network to create
     * @param id Unique identifier for the network
     * @return Shared pointer to the created network
     */
    static std::shared_ptr<Network> createNetwork(NetworkType type, const std::string& id);
};

#endif // NETWORK_H

