/**
 * @file neuron_gate.cpp
 * @brief Implementation of neuron gate classes for pathway control.
 */

#include "../include/neuron_gate.h"
#include "../include/utils.h"
#include <algorithm>
#include <numeric>

// ============== Base NeuronGate Implementation ==============

NeuronGate::NeuronGate(const std::string& id, GateType type)
    : id(id), type(type), threshold(0.5f), active(true), adaptationRate(0.1f) {
}

NeuronGate::~NeuronGate() {
}

NeuronGate::GateType NeuronGate::getType() const {
    return type;
}

const std::string& NeuronGate::getId() const {
    return id;
}

void NeuronGate::setThreshold(float threshold) {
    this->threshold = std::min(1.0f, std::max(0.0f, threshold));
}

float NeuronGate::getThreshold() const {
    return threshold;
}

void NeuronGate::adapt(bool success) {
    // Basic adaptation: adjust threshold based on success/failure
    if (success) {
        // If successful, slightly lower the threshold to make the gate more permissive
        threshold = std::max(0.1f, threshold - adaptationRate);
    } else {
        // If failed, slightly raise the threshold to make the gate more restrictive
        threshold = std::min(0.9f, threshold + adaptationRate);
    }
}

bool NeuronGate::isActive() const {
    return active;
}

void NeuronGate::setActive(bool active) {
    this->active = active;
}

// ============== AndGate Implementation ==============

AndGate::AndGate(const std::string& id)
    : NeuronGate(id, GateType::AND) {
}

std::shared_ptr<Synapse> AndGate::process(const std::vector<std::shared_ptr<Synapse>>& inputs) {
    // If no inputs, return nullptr
    if (inputs.empty()) {
        return nullptr;
    }

    // Check if all inputs are above threshold
    bool allAboveThreshold = true;
    for (const auto& input : inputs) {
        if (!input || input->getStrength() < threshold) {
            allAboveThreshold = false;
            break;
        }
    }

    // If all inputs are above threshold, combine them
    if (allAboveThreshold) {
        // Start with the first input
        auto result = inputs[0]->derive();

        // Calculate average strength
        float totalStrength = 0.0f;
        for (const auto& input : inputs) {
            totalStrength += input->getStrength();
        }
        float avgStrength = totalStrength / inputs.size();

        // Set the result strength
        result->setStrength(avgStrength);

        // Add gate information
        result->setData("gate_id", id);
        result->setData("gate_type", "AND");
        result->addTag("gate_processed");

        return result;
    } else {
        // If any input is below threshold, return nullptr (gate does not activate)
        return nullptr;
    }
}

// ============== OrGate Implementation ==============

OrGate::OrGate(const std::string& id)
    : NeuronGate(id, GateType::OR) {
}

std::shared_ptr<Synapse> OrGate::process(const std::vector<std::shared_ptr<Synapse>>& inputs) {
    // If no inputs, return nullptr
    if (inputs.empty()) {
        return nullptr;
    }

    // Check if any input is above threshold
    std::shared_ptr<Synapse> strongestInput = nullptr;
    float maxStrength = 0.0f;

    for (const auto& input : inputs) {
        if (input && input->getStrength() >= threshold) {
            if (input->getStrength() > maxStrength) {
                maxStrength = input->getStrength();
                strongestInput = input;
            }
        }
    }

    // If at least one input is above threshold, return derived synapse from strongest
    if (strongestInput) {
        auto result = strongestInput->derive();

        // Add gate information
        result->setData("gate_id", id);
        result->setData("gate_type", "OR");
        result->addTag("gate_processed");

        return result;
    } else {
        // If no input is above threshold, return nullptr (gate does not activate)
        return nullptr;
    }
}

// ============== NotGate Implementation ==============

NotGate::NotGate(const std::string& id)
    : NeuronGate(id, GateType::NOT) {
}

std::shared_ptr<Synapse> NotGate::process(const std::vector<std::shared_ptr<Synapse>>& inputs) {
    // Not gate only operates on the first input
    if (inputs.empty() || !inputs[0]) {
        return nullptr;
    }

    auto input = inputs[0];
    auto result = input->derive();

    // Invert the strength
    result->setStrength(1.0f - input->getStrength());

    // Add gate information
    result->setData("gate_id", id);
    result->setData("gate_type", "NOT");
    result->addTag("gate_processed");

    return result;
}

// ============== XorGate Implementation ==============

XorGate::XorGate(const std::string& id)
    : NeuronGate(id, GateType::XOR) {
}

std::shared_ptr<Synapse> XorGate::process(const std::vector<std::shared_ptr<Synapse>>& inputs) {
    // XOR gate requires exactly two inputs
    if (inputs.size() != 2 || !inputs[0] || !inputs[1]) {
        return nullptr;
    }

    float strength1 = inputs[0]->getStrength();
    float strength2 = inputs[1]->getStrength();

    // XOR: One input must be above threshold, the other must be below
    bool input1Above = strength1 >= threshold;
    bool input2Above = strength2 >= threshold;

    if ((input1Above && !input2Above) || (!input1Above && input2Above)) {
        // XOR condition satisfied
        auto result = (input1Above ? inputs[0] : inputs[1])->derive();

        // Set strength proportional to the difference between inputs
        float strengthDiff = std::abs(strength1 - strength2);
        result->setStrength(strengthDiff);

        // Add gate information
        result->setData("gate_id", id);
        result->setData("gate_type", "XOR");
        result->addTag("gate_processed");

        return result;
    } else {
        // XOR condition not satisfied
        return nullptr;
    }
}

// ============== ThresholdGate Implementation ==============

ThresholdGate::ThresholdGate(const std::string& id, float threshold)
    : NeuronGate(id, GateType::THRESHOLD) {
    setThreshold(threshold);
}

std::shared_ptr<Synapse> ThresholdGate::process(const std::vector<std::shared_ptr<Synapse>>& inputs) {
    // Operate on the first input only
    if (inputs.empty() || !inputs[0]) {
        return nullptr;
    }

    auto input = inputs[0];

    // Check if input is above threshold
    if (input->getStrength() >= threshold) {
        auto result = input->derive();

        // Keep the same strength
        result->setStrength(input->getStrength());

        // Add gate information
        result->setData("gate_id", id);
        result->setData("gate_type", "THRESHOLD");
        result->addTag("gate_processed");

        return result;
    } else {
        // Input is below threshold, gate does not activate
        return nullptr;
    }
}

// ============== ModulatorGate Implementation ==============

ModulatorGate::ModulatorGate(const std::string& id, float factor)
    : NeuronGate(id, GateType::MODULATOR), factor(factor) {
}

std::shared_ptr<Synapse> ModulatorGate::process(const std::vector<std::shared_ptr<Synapse>>& inputs) {
    // If no inputs, return nullptr
    if (inputs.empty() || !inputs[0]) {
        return nullptr;
    }

    auto input = inputs[0];
    auto result = input->derive();

    // Modulate the strength by the factor
    result->setStrength(std::min(1.0f, std::max(0.0f, input->getStrength() * factor)));

    // Add gate information
    result->setData("gate_id", id);
    result->setData("gate_type", "MODULATOR");
    result->setData("modulation_factor", std::to_string(factor));
    result->addTag("gate_processed");

    return result;
}

void ModulatorGate::setFactor(float factor) {
    this->factor = factor;
}

float ModulatorGate::getFactor() const {
    return factor;
}

// ============== CustomGate Implementation ==============

CustomGate::CustomGate(const std::string& id,
                     std::function<std::shared_ptr<Synapse>(const std::vector<std::shared_ptr<Synapse>>&)> processor)
    : NeuronGate(id, GateType::CUSTOM), processor(processor) {
}

std::shared_ptr<Synapse> CustomGate::process(const std::vector<std::shared_ptr<Synapse>>& inputs) {
    // Use the custom processor function
    auto result = processor(inputs);

    if (result) {
        // Add gate information
        result->setData("gate_id", id);
        result->setData("gate_type", "CUSTOM");
        result->addTag("gate_processed");
    }

    return result;
}

void CustomGate::setProcessor(const std::function<std::shared_ptr<Synapse>(const std::vector<std::shared_ptr<Synapse>>&)>& processor) {
    this->processor = processor;
}

// ============== NeuronGateFactory Implementation ==============

std::shared_ptr<NeuronGate> NeuronGateFactory::createGate(NeuronGate::GateType type, const std::string& id) {
    switch (type) {
        case NeuronGate::GateType::AND:
            return std::make_shared<AndGate>(id);

        case NeuronGate::GateType::OR:
            return std::make_shared<OrGate>(id);

        case NeuronGate::GateType::NOT:
            return std::make_shared<NotGate>(id);

        case NeuronGate::GateType::XOR:
            return std::make_shared<XorGate>(id);

        case NeuronGate::GateType::THRESHOLD:
            return std::make_shared<ThresholdGate>(id);

        case NeuronGate::GateType::MODULATOR:
            return std::make_shared<ModulatorGate>(id);

        case NeuronGate::GateType::CUSTOM:
            // Default custom gate just passes through the first input
            return std::make_shared<CustomGate>(id,
                [](const std::vector<std::shared_ptr<Synapse>>& inputs) -> std::shared_ptr<Synapse> {
                    if (inputs.empty() || !inputs[0]) {
                        return nullptr;
                    }
                    return inputs[0]->derive();
                });

        default:
            return nullptr;
    }
}

std::shared_ptr<CustomGate> NeuronGateFactory::createCustomGate(
    const std::string& id,
    const std::function<std::shared_ptr<Synapse>(const std::vector<std::shared_ptr<Synapse>>&)> processor) {

    return std::make_shared<CustomGate>(id, processor);
}