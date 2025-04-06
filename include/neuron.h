/**
 * @file neuron.h
 * @brief Definition of the Neuron class for neuromorphic processing.
 * 
 * This file contains the Neuron class that simulates biological
 * neuron behavior in the Ozone (O3) architecture.
 */

#ifndef NEURON_H
#define NEURON_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include "synapse.h"
#include "neuron_gate.h"

/**
 * @brief The Neuron class simulates a biological neuron
 */
class Neuron : public std::enable_shared_from_this<Neuron> {
public:
    /**
     * @brief Types of neurons with different specializations
     */
    enum class NeuronType {
        SENSORY,      // Receives external stimuli
        PROCESSING,   // Processes information
        MEMORY,       // Stores information
        INTEGRATION,  // Integrates multiple inputs
        ASSOCIATION,  // Forms associations between neurons
        OUTPUT,       // Produces output signals
        REGULATORY    // Regulates network activity
    };
    
    /**
     * @brief States that a neuron can be in
     */
    enum class NeuronState {
        RESTING,    // Baseline state
        ACTIVE,     // Firing state
        REFRACTORY, // Recovery state
        INHIBITED   // Suppressed state
    };
    
    /**
     * @brief Constructor for Neuron
     * @param id Unique identifier
     * @param type Type of the neuron
     */
    Neuron(const std::string& id, NeuronType type);
    
    /**
     * @brief Destructor for Neuron
     */
    ~Neuron();
    
    /**
     * @brief Get neuron's unique identifier
     * @return The ID string
     */
    const std::string& getId() const;
    
    /**
     * @brief Get neuron's type
     * @return The type of the neuron
     */
    NeuronType getType() const;
    
    /**
     * @brief Create a new neuron gate for pathway control
     * @param gateType The type of gate to create
     * @return Pointer to the created gate
     */
    std::shared_ptr<NeuronGate> createGate(NeuronGate::GateType gateType);
    
    /**
     * @brief Set the neuron's activation state
     * @param state The new state of the neuron
     */
    void setState(NeuronState state);
    
    /**
     * @brief Get the current neuron state
     * @return The current state
     */
    NeuronState getState() const;
    
    /**
     * @brief Set the neuron's activation threshold
     * @param threshold Activation threshold value (0.0 to 1.0)
     */
    void setThreshold(float threshold);
    
    /**
     * @brief Get the current activation threshold
     * @return The threshold value
     */
    float getThreshold() const;
    
    /**
     * @brief Connect this neuron to another
     * @param target Target neuron to connect to
     * @param weight Initial connection weight
     * @return True if connection was successful
     */
    bool connectTo(std::shared_ptr<Neuron> target, float weight = 1.0f);
    
    /**
     * @brief Disconnect this neuron from another
     * @param target Target neuron to disconnect from
     * @return True if disconnection was successful
     */
    bool disconnectFrom(std::shared_ptr<Neuron> target);
    
    /**
     * @brief Receive a synaptic signal
     * @param signal The incoming synapse
     */
    void receiveSignal(std::shared_ptr<Synapse> signal);
    
    /**
     * @brief Process accumulated signals
     */
    void processSignals();
    
    /**
     * @brief Fire a signal to connected neurons
     */
    void fire();
    
    /**
     * @brief Add a tag to this neuron
     * @param tag The tag to add
     */
    void addTag(const std::string& tag);
    
    /**
     * @brief Check if neuron has a specific tag
     * @param tag The tag to check
     * @return True if neuron has the tag
     */
    bool hasTag(const std::string& tag) const;
    
    /**
     * @brief Get all tags for this neuron
     * @return Vector of tags
     */
    std::vector<std::string> getTags() const;
    
    /**
     * @brief Set neuron metadata
     * @param key Metadata key
     * @param value Metadata value
     */
    void setMetadata(const std::string& key, const std::string& value);
    
    /**
     * @brief Get neuron metadata
     * @param key Metadata key
     * @return Metadata value or empty string if not found
     */
    std::string getMetadata(const std::string& key) const;
    
    /**
     * @brief Check if this neuron has a specific metadata key
     * @param key The key to check
     * @return True if the metadata key exists
     */
    bool hasMetadata(const std::string& key) const;
    
    /**
     * @brief Get current activation potential
     * @return Activation potential (0.0 to 1.0)
     */
    float getPotential() const;
    
    /**
     * @brief Get all incoming connections
     * @return Vector of input connections
     */
    std::vector<std::shared_ptr<Neuron>> getInputs() const;
    
    /**
     * @brief Get all outgoing connections
     * @return Vector of output connections
     */
    std::vector<std::shared_ptr<Neuron>> getOutputs() const;
    
    /**
     * @brief Register a callback for neuron firing
     * @param callback Function to call when neuron fires
     */
    void onFire(std::function<void(std::shared_ptr<Neuron>)> callback);
    
    /**
     * @brief Register a callback for state changes
     * @param callback Function to call when state changes
     */
    void onStateChange(std::function<void(std::shared_ptr<Neuron>, NeuronState, NeuronState)> callback);
    
    /**
     * @brief Get the weight of connection to a specific neuron
     * @param target The target neuron
     * @return The connection weight or 0 if not connected
     */
    float getConnectionWeight(std::shared_ptr<Neuron> target) const;
    
    /**
     * @brief Set the weight for a connection
     * @param target The target neuron
     * @param weight New connection weight
     * @return True if connection exists and weight was updated
     */
    bool setConnectionWeight(std::shared_ptr<Neuron> target, float weight);
    
private:
    std::string id;                // Unique identifier
    NeuronType type;               // Neuron type
    NeuronState state;             // Current state
    float threshold;               // Activation threshold
    float potential;               // Current activation potential
    bool refractoryPeriod;         // Whether in refractory period
    
    std::vector<std::shared_ptr<Synapse>> inputSignals;  // Accumulated input signals
    std::vector<std::shared_ptr<Synapse>> outputSignals; // Output signals
    
    std::map<std::shared_ptr<Neuron>, float> connections;  // Outgoing connections with weights
    std::vector<std::weak_ptr<Neuron>> inputs;             // Incoming connections (weak to avoid circular references)
    
    std::vector<std::string> tags;                   // Tags for categorization
    std::map<std::string, std::string> metadata;     // Additional metadata
    
    std::vector<std::shared_ptr<NeuronGate>> gates;  // Signal processing gates
    
    // Callbacks
    std::vector<std::function<void(std::shared_ptr<Neuron>)>> fireCallbacks;
    std::vector<std::function<void(std::shared_ptr<Neuron>, NeuronState, NeuronState)>> stateChangeCallbacks;
    
    /**
     * @brief Reset the neuron to resting state
     */
    void reset();
    
    /**
     * @brief Integrate incoming signals
     * @return True if threshold is exceeded
     */
    bool integrate();
};

#endif // NEURON_H

