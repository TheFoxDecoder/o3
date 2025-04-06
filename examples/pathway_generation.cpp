/**
 * @file pathway_generation.cpp
 * @brief Example demonstrating dynamic pathway generation in the Ozone (O3) architecture.
 * 
 * This example shows how neurons can dynamically create and modify connections
 * based on input data and learning mechanisms, demonstrating the adaptive nature
 * of the system.
 */

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <random>
#include "../include/neuron.h"
#include "../include/synapse.h"
#include "../include/network.h"
#include "../include/utils.h"
#include "../visualizer/visualizer.h"

/**
 * This example demonstrates how neurons can dynamically create and modify
 * pathways based on input data. It simulates a simple emotional learning
 * system where associations are formed between sensory input and emotional
 * responses.
 */
int pathwayGenerationMain() {
    std::cout << "\n=== Pathway Generation Example ===" << std::endl;
    std::cout << "This example demonstrates how neurons dynamically create connections based on stimuli\n" << std::endl;
    
    // Random number generator for stochastic behavior
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    
    // Create a network
    Network network("Emotional_Learning_Network");
    
    // Create sensory neurons
    auto visualSensor = network.createNeuron("visual_sensor", Neuron::NeuronType::SENSORY);
    auto auditorySensor = network.createNeuron("auditory_sensor", Neuron::NeuronType::SENSORY);
    
    // Create association neurons
    auto visualProcessor = network.createNeuron("visual_processor", Neuron::NeuronType::PROCESSING);
    auto auditoryProcessor = network.createNeuron("auditory_processor", Neuron::NeuronType::PROCESSING);
    auto toneAssociation = network.createNeuron("tone_association", Neuron::NeuronType::ASSOCIATION);
    
    // Create emotional neurons (which will form dynamic connections)
    auto happyEmotion = network.createNeuron("happy_emotion", Neuron::NeuronType::MEMORY);
    auto sadEmotion = network.createNeuron("sad_emotion", Neuron::NeuronType::MEMORY);
    auto angryEmotion = network.createNeuron("angry_emotion", Neuron::NeuronType::MEMORY);
    auto fearEmotion = network.createNeuron("fear_emotion", Neuron::NeuronType::MEMORY);
    
    // Create integration neuron
    auto emotionProcessor = network.createNeuron("emotion_processor", Neuron::NeuronType::INTEGRATION);
    
    // Create output neurons
    auto emotionOutput = network.createNeuron("emotion_output", Neuron::NeuronType::OUTPUT);
    auto behaviorOutput = network.createNeuron("behavior_output", Neuron::NeuronType::OUTPUT);
    
    // Create a regulatory neuron for attention
    auto attentionRegulator = network.createNeuron("attention_regulator", Neuron::NeuronType::REGULATORY);
    
    // Set up initial static connections
    visualSensor->connectTo(visualProcessor, 0.8f);
    auditorySensor->connectTo(auditoryProcessor, 0.7f);
    
    auditoryProcessor->connectTo(toneAssociation, 0.6f);
    
    // Connect emotion neurons to processor
    happyEmotion->connectTo(emotionProcessor, 0.7f);
    sadEmotion->connectTo(emotionProcessor, 0.7f);
    angryEmotion->connectTo(emotionProcessor, 0.7f);
    fearEmotion->connectTo(emotionProcessor, 0.7f);
    
    // Connect to outputs
    emotionProcessor->connectTo(emotionOutput, 0.9f);
    emotionProcessor->connectTo(behaviorOutput, 0.8f);
    
    // Set up attention regulation
    attentionRegulator->connectTo(visualProcessor, 0.5f);
    attentionRegulator->connectTo(auditoryProcessor, 0.5f);
    attentionRegulator->connectTo(toneAssociation, 0.5f);
    
    // Set up custom handlers for dynamic connection formation
    
    // Visual processor dynamically connects to emotion based on coincidence
    visualProcessor->onFire([&](std::shared_ptr<Neuron> neuron) {
        // Check which emotion neurons are currently active
        auto activeEmotions = std::vector<std::shared_ptr<Neuron>>();
        
        if (happyEmotion->getState() == Neuron::NeuronState::ACTIVE) {
            activeEmotions.push_back(happyEmotion);
        }
        if (sadEmotion->getState() == Neuron::NeuronState::ACTIVE) {
            activeEmotions.push_back(sadEmotion);
        }
        if (angryEmotion->getState() == Neuron::NeuronState::ACTIVE) {
            activeEmotions.push_back(angryEmotion);
        }
        if (fearEmotion->getState() == Neuron::NeuronState::ACTIVE) {
            activeEmotions.push_back(fearEmotion);
        }
        
        // If any emotions are active, form or strengthen connections
        for (auto& emotion : activeEmotions) {
            // Check if connection already exists
            float currentWeight = neuron->getConnectionWeight(emotion);
            
            if (currentWeight > 0.0f) {
                // Strengthen existing connection (Hebbian learning)
                float newWeight = std::min(1.0f, currentWeight + 0.1f);
                neuron->updateConnectionWeight(emotion, newWeight);
                
                std::cout << "Strengthened connection from " << neuron->getId() 
                          << " to " << emotion->getId() 
                          << " (weight: " << newWeight << ")" << std::endl;
            } else {
                // Form new connection
                neuron->connectTo(emotion, 0.3f);  // Start with moderate weight
                
                std::cout << "Formed new connection from " << neuron->getId() 
                          << " to " << emotion->getId() << std::endl;
            }
        }
    });
    
    // Tone association also forms dynamic connections to emotions
    toneAssociation->onFire([&](std::shared_ptr<Neuron> neuron) {
        // Check which emotion neurons are currently active
        auto activeEmotions = std::vector<std::shared_ptr<Neuron>>();
        
        if (happyEmotion->getState() == Neuron::NeuronState::ACTIVE) {
            activeEmotions.push_back(happyEmotion);
        }
        if (sadEmotion->getState() == Neuron::NeuronState::ACTIVE) {
            activeEmotions.push_back(sadEmotion);
        }
        if (angryEmotion->getState() == Neuron::NeuronState::ACTIVE) {
            activeEmotions.push_back(fearEmotion);
        }
        if (fearEmotion->getState() == Neuron::NeuronState::ACTIVE) {
            activeEmotions.push_back(fearEmotion);
        }
        
        // If any emotions are active, form or strengthen connections
        for (auto& emotion : activeEmotions) {
            // Check if connection already exists
            float currentWeight = neuron->getConnectionWeight(emotion);
            
            if (currentWeight > 0.0f) {
                // Strengthen existing connection (Hebbian learning)
                float newWeight = std::min(1.0f, currentWeight + 0.15f);  // Tones form stronger associations
                neuron->updateConnectionWeight(emotion, newWeight);
                
                std::cout << "Strengthened connection from " << neuron->getId() 
                          << " to " << emotion->getId() 
                          << " (weight: " << newWeight << ")" << std::endl;
            } else {
                // Form new connection
                neuron->connectTo(emotion, 0.4f);  // Start with moderate weight
                
                std::cout << "Formed new connection from " << neuron->getId() 
                          << " to " << emotion->getId() << std::endl;
            }
        }
    });
    
    // Create visualizer
    Visualizer visualizer(800, 600);
    
    // Add neurons to visualizer
    visualizer.addNeuron(visualSensor, 0.1f, 0.3f, 10, "blue");
    visualizer.addNeuron(auditorySensor, 0.1f, 0.7f, 10, "blue");
    
    visualizer.addNeuron(visualProcessor, 0.3f, 0.3f, 10, "yellow");
    visualizer.addNeuron(auditoryProcessor, 0.3f, 0.7f, 10, "yellow");
    visualizer.addNeuron(toneAssociation, 0.5f, 0.7f, 10, "orange");
    
    visualizer.addNeuron(happyEmotion, 0.6f, 0.2f, 10, "pink");
    visualizer.addNeuron(sadEmotion, 0.6f, 0.4f, 10, "blue");
    visualizer.addNeuron(angryEmotion, 0.6f, 0.6f, 10, "red");
    visualizer.addNeuron(fearEmotion, 0.6f, 0.8f, 10, "purple");
    
    visualizer.addNeuron(emotionProcessor, 0.8f, 0.5f, 12, "gray");
    visualizer.addNeuron(emotionOutput, 0.9f, 0.3f, 10, "green");
    visualizer.addNeuron(behaviorOutput, 0.9f, 0.7f, 10, "green");
    
    visualizer.addNeuron(attentionRegulator, 0.4f, 0.5f, 8, "white");
    
    // Add initial connections to visualizer
    visualizer.addConnection(visualSensor->getId(), visualProcessor->getId());
    visualizer.addConnection(auditorySensor->getId(), auditoryProcessor->getId());
    visualizer.addConnection(auditoryProcessor->getId(), toneAssociation->getId());
    
    visualizer.addConnection(happyEmotion->getId(), emotionProcessor->getId());
    visualizer.addConnection(sadEmotion->getId(), emotionProcessor->getId());
    visualizer.addConnection(angryEmotion->getId(), emotionProcessor->getId());
    visualizer.addConnection(fearEmotion->getId(), emotionProcessor->getId());
    
    visualizer.addConnection(emotionProcessor->getId(), emotionOutput->getId());
    visualizer.addConnection(emotionProcessor->getId(), behaviorOutput->getId());
    
    visualizer.addConnection(attentionRegulator->getId(), visualProcessor->getId());
    visualizer.addConnection(attentionRegulator->getId(), auditoryProcessor->getId());
    visualizer.addConnection(attentionRegulator->getId(), toneAssociation->getId());
    
    // Display initial network
    std::cout << "\nInitial network state (before learning):" << std::endl;
    visualizer.show();
    
    // Training phase - associate stimuli with emotions
    std::cout << "\n=== Training Phase ===" << std::endl;
    std::cout << "Running 3 training scenarios to form emotional associations...\n" << std::endl;
    
    // Scenario 1: Visual input (light) + happiness
    std::cout << "Scenario 1: Visual input (light) + happiness" << std::endl;
    
    // Activate visual sensor with light stimulus
    auto lightSignal = std::make_shared<Synapse>("light_signal");
    lightSignal->setData("type", std::string("light"));
    lightSignal->setData("intensity", std::string("high"));
    lightSignal->setStrength(0.9f);
    visualSensor->receiveSignal(lightSignal);
    
    // Directly activate happy emotion (as if from some other input)
    auto happySignal = std::make_shared<Synapse>("happy_signal");
    happySignal->setStrength(0.8f);
    happyEmotion->receiveSignal(happySignal);
    
    // Process signals
    network.processSignals();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Scenario 2: Auditory input (tone) + fear
    std::cout << "\nScenario 2: Auditory input (tone) + fear" << std::endl;
    
    // Reset neuron states
    visualSensor->setState(Neuron::NeuronState::RESTING);
    visualProcessor->setState(Neuron::NeuronState::RESTING);
    happyEmotion->setState(Neuron::NeuronState::RESTING);
    
    // Activate auditory sensor with tone stimulus
    auto toneSignal = std::make_shared<Synapse>("tone_signal");
    toneSignal->setData("type", std::string("tone"));
    toneSignal->setData("frequency", std::string("high"));
    toneSignal->setStrength(0.85f);
    auditorySensor->receiveSignal(toneSignal);
    
    // Directly activate fear emotion
    auto fearSignal = std::make_shared<Synapse>("fear_signal");
    fearSignal->setStrength(0.9f);
    fearEmotion->receiveSignal(fearSignal);
    
    // Process signals
    network.processSignals();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Scenario 3: Visual + auditory + anger (multimodal association)
    std::cout << "\nScenario 3: Visual + auditory inputs + anger (multimodal)" << std::endl;
    
    // Reset neuron states
    auditorySensor->setState(Neuron::NeuronState::RESTING);
    auditoryProcessor->setState(Neuron::NeuronState::RESTING);
    toneAssociation->setState(Neuron::NeuronState::RESTING);
    fearEmotion->setState(Neuron::NeuronState::RESTING);
    
    // Activate both visual and auditory sensors
    visualSensor->receiveSignal(lightSignal);
    auditorySensor->receiveSignal(toneSignal);
    
    // Activate anger emotion
    auto angerSignal = std::make_shared<Synapse>("anger_signal");
    angerSignal->setStrength(0.75f);
    angryEmotion->receiveSignal(angerSignal);
    
    // Boost attention for multimodal learning
    auto attentionSignal = std::make_shared<Synapse>("attention_signal");
    attentionSignal->setStrength(0.8f);
    attentionRegulator->receiveSignal(attentionSignal);
    
    // Process signals
    network.processSignals();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Show network after learning
    std::cout << "\nNetwork state after learning:" << std::endl;
    visualizer.update();
    visualizer.show();
    
    // Testing phase - see if formed associations trigger correct emotions
    std::cout << "\n=== Testing Phase ===" << std::endl;
    std::cout << "Testing if sensory inputs now trigger associated emotions...\n" << std::endl;
    
    // Reset network
    network.reset();
    
    // Test 1: Visual stimulus alone
    std::cout << "Test 1: Visual stimulus (light) alone" << std::endl;
    visualSensor->receiveSignal(lightSignal);
    
    // Process and observe results
    network.processSignals();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Check if happy emotion activated
    std::cout << "Happy emotion state: " << 
        (happyEmotion->getState() == Neuron::NeuronState::ACTIVE ? "ACTIVE" : "INACTIVE") << 
        " (Potential: " << happyEmotion->getPotential() << ")" << std::endl;
    
    // Test 2: Auditory stimulus alone
    std::cout << "\nTest 2: Auditory stimulus (tone) alone" << std::endl;
    
    // Reset network
    network.reset();
    
    // Send tone signal
    auditorySensor->receiveSignal(toneSignal);
    
    // Process and observe results
    network.processSignals();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Check if fear emotion activated
    std::cout << "Fear emotion state: " << 
        (fearEmotion->getState() == Neuron::NeuronState::ACTIVE ? "ACTIVE" : "INACTIVE") << 
        " (Potential: " << fearEmotion->getPotential() << ")" << std::endl;
    
    // Final network state
    visualizer.update();
    visualizer.show();
    
    std::cout << "\nPathway generation demo completed." << std::endl;
    
    return 0;
}

// This allows the example to be compiled as a standalone executable
// or called from the main program
int main() {
    return pathwayGenerationMain();
}

