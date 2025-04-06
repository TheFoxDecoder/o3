/**
 * @file synapse.cpp
 * @brief Implementation of the synapse class for data transfer between neurons.
 */

#include "../include/synapse.h"
#include "../include/utils.h"
#include <sstream>
#include <iomanip>
#include <functional>
#include <algorithm>

Synapse::Synapse(SynapseType type, float strength)
    : id(Utils::generateUUID()), type(type), strength(std::min(1.0f, std::max(0.0f, strength))) {
}

Synapse::Synapse(const std::string& id, SynapseType type, float strength)
    : id(id), type(type), strength(std::min(1.0f, std::max(0.0f, strength))) {
}

void Synapse::setSourceId(const std::string& sourceId) {
    this->sourceId = sourceId;
}

void Synapse::setTargetId(const std::string& targetId) {
    this->targetId = targetId;
}

const std::string& Synapse::getSourceId() const {
    return sourceId;
}

const std::string& Synapse::getTargetId() const {
    return targetId;
}

const std::string& Synapse::getId() const {
    return id;
}

Synapse::SynapseType Synapse::getType() const {
    return type;
}

float Synapse::getStrength() const {
    return strength;
}

void Synapse::setStrength(float value) {
    strength = std::min(1.0f, std::max(0.0f, value));
}

bool Synapse::hasData(const std::string& key) const {
    return stringPayload.find(key) != stringPayload.end();
}

std::vector<std::string> Synapse::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(stringPayload.size());
    
    for (const auto& pair : stringPayload) {
        keys.push_back(pair.first);
    }
    
    return keys;
}

void Synapse::addTag(const std::string& tag) {
    // Check if tag already exists
    if (std::find(tags.begin(), tags.end(), tag) == tags.end()) {
        tags.push_back(tag);
    }
}

bool Synapse::hasTag(const std::string& tag) const {
    return std::find(tags.begin(), tags.end(), tag) != tags.end();
}

const std::vector<std::string>& Synapse::getTags() const {
    return tags;
}

std::shared_ptr<Synapse> Synapse::derive(float derivedStrength) const {
    // Create a new synapse with the same type but potentially different strength
    float newStrength = (derivedStrength >= 0.0f) ? derivedStrength : strength;
    auto derived = std::make_shared<Synapse>(Utils::generateUUID(), type, newStrength);
    
    // Copy source and target
    derived->setSourceId(sourceId);
    derived->setTargetId(targetId);
    
    // Copy tags
    for (const auto& tag : tags) {
        derived->addTag(tag);
    }
    
    // Add derivation metadata
    derived->setData("derived_from", id);
    
    return derived;
}

std::shared_ptr<Synapse> Synapse::combine(const std::shared_ptr<Synapse>& other) const {
    if (!other) {
        return derive();  // Just derive from this if other is null
    }
    
    // Create a new synapse with combined properties
    float combinedStrength = (strength + other->getStrength()) / 2.0f;  // Average strength
    auto combined = std::make_shared<Synapse>(Utils::generateUUID(), type, combinedStrength);
    
    // Set source as this synapse's source
    combined->setSourceId(sourceId);
    
    // Set target as other synapse's target
    combined->setTargetId(other->getTargetId());
    
    // Combine tags
    for (const auto& tag : tags) {
        combined->addTag(tag);
    }
    
    for (const auto& tag : other->getTags()) {
        combined->addTag(tag);
    }
    
    // Add combination metadata
    combined->setData("combined_from_1", id);
    combined->setData("combined_from_2", other->getId());
    
    // Combine selected data (implementation-specific)
    // This is a simplistic approach; a real implementation would be more sophisticated
    auto keys1 = getKeys();
    auto keys2 = other->getKeys();
    
    for (const auto& key : keys1) {
        // Skip metadata that shouldn't be combined
        if (key == "source" || key == "target" || key == "strength") {
            continue;
        }
        
        // For simplicity, just copy string data
        std::string value = getData<std::string>(key);
        combined->setData(key + "_1", value);
    }
    
    for (const auto& key : keys2) {
        // Skip metadata that shouldn't be combined
        if (key == "source" || key == "target" || key == "strength") {
            continue;
        }
        
        // For simplicity, just copy string data
        std::string value = other->getData<std::string>(key);
        combined->setData(key + "_2", value);
    }
    
    return combined;
}

std::string Synapse::createDigitalSignature() const {
    // Create a simple hash-based signature for the synapse
    // In a real implementation, this would be a cryptographic signature
    
    std::stringstream ss;
    
    // Add basic properties
    ss << id << sourceId << targetId << static_cast<int>(type) << strength;
    
    // Add tags
    for (const auto& tag : tags) {
        ss << tag;
    }
    
    // Add payload keys and values
    for (const auto& pair : stringPayload) {
        ss << pair.first << pair.second;
    }
    
    // Create a simple hash of the concatenated data
    // In a real implementation, use a proper cryptographic hash
    return Utils::simpleHash(ss.str());
}

