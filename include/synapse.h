/**
 * @file synapse.h
 * @brief Definition of synapse class for data transfer between neurons.
 * 
 * This file contains the synapse class which is responsible for transferring
 * data between neurons in the Ozone (O3) architecture. It implements the
 * concept of synapse transfer mentioned in the paper.
 */

#ifndef SYNAPSE_H
#define SYNAPSE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <memory>
#include <typeindex>
#include <typeinfo>
#include <utility>  // For std::pair

/**
 * @brief Class representing a synapse for data transfer between neurons.
 */
class Synapse {
public:
    enum class SynapseType {
        EXCITATORY,     // Excitatory synapse (increases activation)
        INHIBITORY,     // Inhibitory synapse (decreases activation)
        MODULATORY      // Modulatory synapse (changes behavior)
    };
    
    /**
     * @brief Constructor for Synapse class
     * @param type Type of the synapse
     * @param strength Strength of the synapse signal (0.0 to 1.0)
     */
    Synapse(SynapseType type = SynapseType::EXCITATORY, float strength = 1.0f);
    
    /**
     * @brief Constructor for Synapse with ID
     * @param id The ID for this synapse
     * @param type Type of the synapse
     * @param strength Strength of the synapse signal (0.0 to 1.0)
     */
    Synapse(const std::string& id, SynapseType type = SynapseType::EXCITATORY, float strength = 1.0f);
    
    /**
     * @brief Set the source ID (the neuron that created this synapse)
     * @param sourceId The ID of the source neuron
     */
    void setSourceId(const std::string& sourceId);
    
    /**
     * @brief Set the target ID (the neuron this synapse is intended for)
     * @param targetId The ID of the target neuron
     */
    void setTargetId(const std::string& targetId);
    
    /**
     * @brief Get the source ID
     * @return The source neuron ID
     */
    const std::string& getSourceId() const;
    
    /**
     * @brief Get the target ID
     * @return The target neuron ID
     */
    const std::string& getTargetId() const;
    
    /**
     * @brief Get the synapse type
     * @return The type of this synapse
     */
    SynapseType getType() const;
    
    /**
     * @brief Get the synapse strength
     * @return The strength value (0.0 to 1.0)
     */
    float getStrength() const;
    
    /**
     * @brief Set the synapse strength
     * @param value The new strength value (0.0 to 1.0)
     */
    void setStrength(float value);
    
    /**
     * @brief Add a data value to the synapse payload
     * @param key The key identifier for the data
     * @param value The data value
     */
    template<typename T>
    void setData(const std::string& key, const T& value) {
        // Store data as string representation for now
        // In C++17, this would use std::any
        if constexpr (std::is_same<T, std::string>::value) {
            stringPayload[key] = value;
        } else {
            // Simple conversion to string for basic types
            stringPayload[key] = std::to_string(value);
        }
    }
    
    /**
     * @brief Retrieve a data value from the synapse payload
     * @param key The key identifier for the data
     * @return The data value or default-constructed T if not found
     */
    template<typename T>
    T getData(const std::string& key) const {
        auto it = stringPayload.find(key);
        if (it != stringPayload.end()) {
            if constexpr (std::is_same<T, std::string>::value) {
                return it->second;
            } else if constexpr (std::is_same<T, int>::value) {
                try {
                    return std::stoi(it->second);
                } catch (...) {
                    return 0;
                }
            } else if constexpr (std::is_same<T, float>::value) {
                try {
                    return std::stof(it->second);
                } catch (...) {
                    return 0.0f;
                }
            } else if constexpr (std::is_same<T, double>::value) {
                try {
                    return std::stod(it->second);
                } catch (...) {
                    return 0.0;
                }
            } else if constexpr (std::is_same<T, bool>::value) {
                return it->second == "true" || it->second == "1";
            }
        }
        
        // Default construction for unhandled types or missing keys
        return T();
    }
    
    /**
     * @brief Check if the synapse contains a specific data key
     * @param key The key to check for
     * @return True if the key exists, false otherwise
     */
    bool hasData(const std::string& key) const;
    
    /**
     * @brief Get all keys in the payload
     * @return Vector of all keys
     */
    std::vector<std::string> getKeys() const;
    
    /**
     * @brief Add a tag to the synapse for categorization
     * @param tag The tag to add
     */
    void addTag(const std::string& tag);
    
    /**
     * @brief Check if the synapse has a specific tag
     * @param tag The tag to check for
     * @return True if the tag exists, false otherwise
     */
    bool hasTag(const std::string& tag) const;
    
    /**
     * @brief Get all tags associated with this synapse
     * @return Vector of all tags
     */
    const std::vector<std::string>& getTags() const;
    
    /**
     * @brief Create a derived synapse based on this one
     * @param strength The strength for the new synapse (defaults to current strength)
     * @return A new synapse with copied metadata and empty payload
     */
    std::shared_ptr<Synapse> derive(float strength = -1.0f) const;
    
    /**
     * @brief Combine this synapse with another
     * @param other The other synapse to combine with
     * @return A new synapse with combined data
     */
    std::shared_ptr<Synapse> combine(const std::shared_ptr<Synapse>& other) const;
    
    /**
     * @brief Record a digital signature of this synapse
     * @return A string representation of the signature
     */
    std::string createDigitalSignature() const;
    
    /**
     * @brief Get the ID of this synapse
     * @return The synapse ID
     */
    const std::string& getId() const;

private:
    std::string id;        // Unique identifier
    std::string sourceId;  // ID of the source neuron
    std::string targetId;  // ID of the target neuron
    SynapseType type;      // Type of the synapse
    float strength;        // Strength of the synapse (0.0 to 1.0)
    
    // Simplified payload storage for C++11 compatibility
    std::unordered_map<std::string, std::string> stringPayload;
    
    // Tags for categorization and filtering
    std::vector<std::string> tags;
};

#endif // SYNAPSE_H

