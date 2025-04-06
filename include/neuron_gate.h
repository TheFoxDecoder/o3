/**
 * @file neuron_gate.h
 * @brief Definition of neuron gate classes for pathway control.
 * 
 * This file contains the neuron gate classes that control data flow
 * between neurons, as described in the Ozone (O3) architecture.
 * These gates are inspired by logic gates but can be dynamically
 * reconfigured based on data flow.
 */

#ifndef NEURON_GATE_H
#define NEURON_GATE_H

#include <memory>
#include <vector>
#include <string>
#include <functional>
#include "synapse.h"

// Forward declaration
class CustomGate;

/**
 * @brief Base class for all neuron gates
 */
class NeuronGate {
public:
    /**
     * @brief Gate types enumeration
     */
    enum class GateType {
        AND,        // Logical AND - all inputs must be active
        OR,         // Logical OR - at least one input must be active
        NOT,        // Logical NOT - inverts the input
        XOR,        // Logical XOR - exactly one input must be active
        THRESHOLD,  // Threshold - input must exceed threshold
        MODULATOR,  // Modulator - adjusts input strength
        CUSTOM      // Custom - user-defined processing
    };
    
    /**
     * @brief Constructor for NeuronGate
     * @param id Unique identifier
     * @param type Type of gate
     */
    NeuronGate(const std::string& id, GateType type);
    
    /**
     * @brief Virtual destructor
     */
    virtual ~NeuronGate();
    
    /**
     * @brief Get the gate type
     * @return The gate type
     */
    GateType getType() const;
    
    /**
     * @brief Get the gate ID
     * @return The unique ID of this gate
     */
    const std::string& getId() const;
    
    /**
     * @brief Set the threshold for activation
     * @param threshold The threshold value (0.0 to 1.0)
     */
    void setThreshold(float threshold);
    
    /**
     * @brief Get the current threshold
     * @return The threshold value
     */
    float getThreshold() const;
    
    /**
     * @brief Adapt the gate based on success/failure
     * @param success Whether the gate's operation was successful
     */
    virtual void adapt(bool success);
    
    /**
     * @brief Check if the gate is active
     * @return True if active, false otherwise
     */
    bool isActive() const;
    
    /**
     * @brief Set the gate's active state
     * @param active The new active state
     */
    void setActive(bool active);
    
    /**
     * @brief Process inputs through the gate
     * @param inputs Vector of input synapses
     * @return Output synapse after processing
     */
    virtual std::shared_ptr<Synapse> process(const std::vector<std::shared_ptr<Synapse>>& inputs) = 0;
    
protected:
    std::string id;  // Unique identifier
    GateType type;   // Gate type
    float threshold; // Activation threshold
    bool active;     // Whether the gate is active
    float adaptationRate; // Rate of adaptation
};

/**
 * @brief AND gate - all inputs must be active
 */
class AndGate : public NeuronGate {
public:
    /**
     * @brief Constructor for AndGate
     * @param id Unique identifier
     */
    AndGate(const std::string& id);
    
    /**
     * @brief Process inputs through AND logic
     * @param inputs Vector of input synapses
     * @return Output synapse after processing
     */
    std::shared_ptr<Synapse> process(const std::vector<std::shared_ptr<Synapse>>& inputs) override;
};

/**
 * @brief OR gate - at least one input must be active
 */
class OrGate : public NeuronGate {
public:
    /**
     * @brief Constructor for OrGate
     * @param id Unique identifier
     */
    OrGate(const std::string& id);
    
    /**
     * @brief Process inputs through OR logic
     * @param inputs Vector of input synapses
     * @return Output synapse after processing
     */
    std::shared_ptr<Synapse> process(const std::vector<std::shared_ptr<Synapse>>& inputs) override;
};

/**
 * @brief NOT gate - inverts the input
 */
class NotGate : public NeuronGate {
public:
    /**
     * @brief Constructor for NotGate
     * @param id Unique identifier
     */
    NotGate(const std::string& id);
    
    /**
     * @brief Process inputs through NOT logic
     * @param inputs Vector of input synapses
     * @return Output synapse after processing
     */
    std::shared_ptr<Synapse> process(const std::vector<std::shared_ptr<Synapse>>& inputs) override;
};

/**
 * @brief XOR gate - exactly one input must be active
 */
class XorGate : public NeuronGate {
public:
    /**
     * @brief Constructor for XorGate
     * @param id Unique identifier
     */
    XorGate(const std::string& id);
    
    /**
     * @brief Process inputs through XOR logic
     * @param inputs Vector of input synapses
     * @return Output synapse after processing
     */
    std::shared_ptr<Synapse> process(const std::vector<std::shared_ptr<Synapse>>& inputs) override;
};

/**
 * @brief Threshold gate - input must exceed threshold
 */
class ThresholdGate : public NeuronGate {
public:
    /**
     * @brief Constructor for ThresholdGate
     * @param id Unique identifier
     * @param threshold Initial threshold value
     */
    ThresholdGate(const std::string& id, float threshold = 0.5f);
    
    /**
     * @brief Process inputs through threshold logic
     * @param inputs Vector of input synapses
     * @return Output synapse after processing
     */
    std::shared_ptr<Synapse> process(const std::vector<std::shared_ptr<Synapse>>& inputs) override;
};

/**
 * @brief Modulator gate - adjusts input strength
 */
class ModulatorGate : public NeuronGate {
public:
    /**
     * @brief Constructor for ModulatorGate
     * @param id Unique identifier
     * @param factor The modulation factor
     */
    ModulatorGate(const std::string& id, float factor = 1.0f);
    
    /**
     * @brief Process inputs by modulating strength
     * @param inputs Vector of input synapses
     * @return Output synapse after processing
     */
    std::shared_ptr<Synapse> process(const std::vector<std::shared_ptr<Synapse>>& inputs) override;
    
    /**
     * @brief Set the modulation factor
     * @param factor The new modulation factor
     */
    void setFactor(float factor);
    
    /**
     * @brief Get the current modulation factor
     * @return The modulation factor
     */
    float getFactor() const;
    
private:
    float factor;  // Modulation factor
};

/**
 * @brief Custom gate - user-defined processing
 */
class CustomGate : public NeuronGate {
public:
    /**
     * @brief Constructor for CustomGate
     * @param id Unique identifier
     * @param processor Function that processes inputs to produce output
     */
    CustomGate(const std::string& id, 
               std::function<std::shared_ptr<Synapse>(const std::vector<std::shared_ptr<Synapse>>&)> processor);
    
    /**
     * @brief Process inputs through custom logic
     * @param inputs Vector of input synapses
     * @return Output synapse after processing
     */
    std::shared_ptr<Synapse> process(const std::vector<std::shared_ptr<Synapse>>& inputs) override;
    
    /**
     * @brief Set the processor function
     * @param processor Function that processes inputs to produce output
     */
    void setProcessor(const std::function<std::shared_ptr<Synapse>(const std::vector<std::shared_ptr<Synapse>>&)>& processor);

private:
    std::function<std::shared_ptr<Synapse>(const std::vector<std::shared_ptr<Synapse>>&)> processor;
};

/**
 * @brief Factory class to create various types of gates
 */
class NeuronGateFactory {
public:
    /**
     * @brief Create a gate of the specified type
     * @param type The type of gate to create
     * @param id Unique identifier for the gate
     * @return Shared pointer to the created gate
     */
    static std::shared_ptr<NeuronGate> createGate(NeuronGate::GateType type, const std::string& id);
    
    /**
     * @brief Create a custom gate with user-defined function
     * @param id Unique identifier for the gate
     * @param processor Function that processes inputs to produce output
     * @return Shared pointer to the created custom gate
     */
    static std::shared_ptr<CustomGate> createCustomGate(
        const std::string& id,
        std::function<std::shared_ptr<Synapse>(const std::vector<std::shared_ptr<Synapse>>&)> processor);
};

#endif // NEURON_GATE_H

