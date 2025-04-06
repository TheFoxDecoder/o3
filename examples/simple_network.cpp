/**
 * @file simple_network.cpp
 * @brief Example of a simple neuromorphic network with the Ozone (O3) architecture.
 * 
 * This example demonstrates the creation of a basic neuromorphic network
 * with various types of neurons, connections between them, and signal processing.
 */

#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <chrono>
#include "../include/neuron.h"
#include "../include/synapse.h"
#include "../include/network.h"
#include "../include/utils.h"
#include "../visualizer/visualizer.h"

/**
 * @brief Class demonstrating a simple sensor neuron implementation
 */
class SensorNeuron {
public:
    SensorNeuron(std::shared_ptr<Neuron> neuron, const std::string& sensorType) 
        : neuron(neuron), sensorType(sensorType) {
        
        neuron->addTag("sensor");
        neuron->addTag(sensorType);
    }
    
    /**
     * @brief Receive sensory input and send a signal
     * @param inputValue Value from the sensor (0.0 to 1.0)
     */
    void receiveInput(float inputValue) {
        // Create a synapse with the sensory data
        auto signal = std::make_shared<Synapse>("sensor_signal");
        signal->setData("sensor_type", sensorType);
        signal->setData("value", std::to_string(inputValue));
        signal->setStrength(inputValue);
        
        // Send the signal to the neuron
        neuron->receiveSignal(signal);
        
        std::cout << "Sensor neuron " << neuron->getId() << " received input: " 
                  << inputValue << std::endl;
    }
    
private:
    std::shared_ptr<Neuron> neuron;
    std::string sensorType;
};

/**
 * @brief Class demonstrating a simple motor neuron implementation
 */
class MotorNeuron {
public:
    MotorNeuron(std::shared_ptr<Neuron> neuron, const std::string& motorType) 
        : neuron(neuron), motorType(motorType), lastActivation(0.0f) {
        
        neuron->addTag("motor");
        neuron->addTag(motorType);
        
        // Register callback for when the neuron fires
        neuron->onFire([this](std::shared_ptr<Neuron> n) {
            this->activate(n->getPotential());
        });
    }
    
    /**
     * @brief Activate the motor with a given strength
     * @param strength Activation strength (0.0 to 1.0)
     */
    void activate(float strength) {
        lastActivation = strength;
        std::cout << "Motor neuron " << neuron->getId() << " activated with strength: " 
                  << strength << std::endl;
        // In a real system, this would control a physical motor or actuator
    }
    
    /**
     * @brief Get the last activation strength
     * @return Last activation value
     */
    float getLastActivation() const {
        return lastActivation;
    }
    
private:
    std::shared_ptr<Neuron> neuron;
    std::string motorType;
    float lastActivation;
};

/**
 * @brief Class demonstrating a simple interneuron implementation
 * that performs basic processing
 */
class ProcessingNeuron {
public:
    ProcessingNeuron(std::shared_ptr<Neuron> neuron, const std::string& processingType) 
        : neuron(neuron), processingType(processingType) {
        
        neuron->addTag("processing");
        neuron->addTag(processingType);
        
        // Set up appropriate threshold based on processing type
        if (processingType == "threshold") {
            neuron->setThreshold(0.5f);
        } else if (processingType == "integrator") {
            neuron->setThreshold(0.3f);
        } else if (processingType == "differentiator") {
            neuron->setThreshold(0.7f);
        }
        
        // Create a gate for signal processing
        auto gate = neuron->createGate(NeuronGate::GateType::THRESHOLD);
    }
    
private:
    std::shared_ptr<Neuron> neuron;
    std::string processingType;
};

/**
 * This example demonstrates a simple neuromorphic network consisting of:
 * 1. Sensory neurons (light, temperature, touch)
 * 2. Processing neurons
 * 3. Motor neurons (arm, leg)
 * 
 * The network implements a basic reflex-like response where high
 * touch input causes immediate motor response.
 */
int simpleNetworkMain() {
    // Create a network
    Network network("Simple_Reflex_Network");
    
    // Create sensory neurons
    auto lightSensorNeuron = network.createNeuron("light_sensor", Neuron::NeuronType::SENSORY);
    auto tempSensorNeuron = network.createNeuron("temp_sensor", Neuron::NeuronType::SENSORY);
    auto touchSensorNeuron = network.createNeuron("touch_sensor", Neuron::NeuronType::SENSORY);
    
    // Create processing neurons
    auto visualProcessor = network.createNeuron("visual_processor", Neuron::NeuronType::PROCESSING);
    auto thermalProcessor = network.createNeuron("thermal_processor", Neuron::NeuronType::PROCESSING);
    auto tactileProcessor = network.createNeuron("tactile_processor", Neuron::NeuronType::PROCESSING);
    auto integrationNeuron = network.createNeuron("integration", Neuron::NeuronType::INTEGRATION);
    
    // Create motor neurons
    auto armMotorNeuron = network.createNeuron("arm_motor", Neuron::NeuronType::OUTPUT);
    auto legMotorNeuron = network.createNeuron("leg_motor", Neuron::NeuronType::OUTPUT);
    
    // Connect sensory to processing
    lightSensorNeuron->connectTo(visualProcessor, 0.8f);
    tempSensorNeuron->connectTo(thermalProcessor, 0.7f);
    touchSensorNeuron->connectTo(tactileProcessor, 0.9f); // Touch has strong connection
    
    // Connect processing to integration
    visualProcessor->connectTo(integrationNeuron, 0.6f);
    thermalProcessor->connectTo(integrationNeuron, 0.6f);
    tactileProcessor->connectTo(integrationNeuron, 0.8f);
    
    // Connect integration to motor neurons
    integrationNeuron->connectTo(armMotorNeuron, 0.7f);
    integrationNeuron->connectTo(legMotorNeuron, 0.5f);
    
    // Create direct reflex pathway (touch sensor directly to motor neuron for fast response)
    touchSensorNeuron->connectTo(armMotorNeuron, 0.95f);
    
    // Wrap neurons in specialized classes
    SensorNeuron lightSensor(lightSensorNeuron, "light");
    SensorNeuron tempSensor(tempSensorNeuron, "temperature");
    SensorNeuron touchSensor(touchSensorNeuron, "touch");
    
    ProcessingNeuron visualProc(visualProcessor, "integrator");
    ProcessingNeuron thermalProc(thermalProcessor, "threshold");
    ProcessingNeuron tactileProc(tactileProcessor, "differentiator");
    
    MotorNeuron armMotor(armMotorNeuron, "arm");
    MotorNeuron legMotor(legMotorNeuron, "leg");
    
    // Create visualizer
    Visualizer visualizer(800, 600);
    
    // Add neurons to visualizer with positions
    visualizer.addNeuron(lightSensorNeuron, 0.1f, 0.2f, 10, "blue");
    visualizer.addNeuron(tempSensorNeuron, 0.1f, 0.5f, 10, "blue");
    visualizer.addNeuron(touchSensorNeuron, 0.1f, 0.8f, 10, "blue");
    
    visualizer.addNeuron(visualProcessor, 0.4f, 0.2f, 10, "yellow");
    visualizer.addNeuron(thermalProcessor, 0.4f, 0.5f, 10, "yellow");
    visualizer.addNeuron(tactileProcessor, 0.4f, 0.8f, 10, "yellow");
    visualizer.addNeuron(integrationNeuron, 0.7f, 0.5f, 12, "purple");
    
    visualizer.addNeuron(armMotorNeuron, 0.9f, 0.3f, 10, "green");
    visualizer.addNeuron(legMotorNeuron, 0.9f, 0.7f, 10, "green");
    
    // Add connections to visualizer
    visualizer.addConnection(lightSensorNeuron->getId(), visualProcessor->getId());
    visualizer.addConnection(tempSensorNeuron->getId(), thermalProcessor->getId());
    visualizer.addConnection(touchSensorNeuron->getId(), tactileProcessor->getId());
    visualizer.addConnection(touchSensorNeuron->getId(), armMotorNeuron->getId(), 0.95f, "red"); // Highlight reflex path
    
    visualizer.addConnection(visualProcessor->getId(), integrationNeuron->getId());
    visualizer.addConnection(thermalProcessor->getId(), integrationNeuron->getId());
    visualizer.addConnection(tactileProcessor->getId(), integrationNeuron->getId());
    
    visualizer.addConnection(integrationNeuron->getId(), armMotorNeuron->getId());
    visualizer.addConnection(integrationNeuron->getId(), legMotorNeuron->getId());
    
    // Display network diagram
    visualizer.show();
    
    // Simulate inputs
    std::cout << "\nStarting simulation with inputs..." << std::endl;
    
    // Send some sample inputs
    std::cout << "\n--- Scenario 1: Low-intensity inputs ---" << std::endl;
    lightSensor.receiveInput(0.3f);
    tempSensor.receiveInput(0.2f);
    touchSensor.receiveInput(0.1f);
    
    // Process signals
    network.processSignals();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Process results:" << std::endl;
    std::cout << "Arm activation: " << armMotor.getLastActivation() << std::endl;
    std::cout << "Leg activation: " << legMotor.getLastActivation() << std::endl;
    
    // Now try with high touch input to trigger reflex
    std::cout << "\n--- Scenario 2: High touch input (reflex) ---" << std::endl;
    lightSensor.receiveInput(0.3f);
    tempSensor.receiveInput(0.2f);
    touchSensor.receiveInput(0.9f);  // High touch should trigger reflex
    
    // Process signals
    network.processSignals();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    std::cout << "Process results:" << std::endl;
    std::cout << "Arm activation: " << armMotor.getLastActivation() << std::endl;
    std::cout << "Leg activation: " << legMotor.getLastActivation() << std::endl;
    
    // Show state of network after processing
    visualizer.update();
    visualizer.show();
    
    std::cout << "\nSimple network demo completed." << std::endl;
    
    return 0;
}

// This allows the example to be compiled as a standalone executable
// or called from the main program
int main() {
    return simpleNetworkMain();
}

