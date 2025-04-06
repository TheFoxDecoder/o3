/**
 * @file utils.h
 * @brief Utility functions and classes for the Ozone (O3) architecture.
 * 
 * This file contains various utility functions and classes that are used
 * throughout the Ozone (O3) implementation.
 */

#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <random>
#include <chrono>
#include <vector>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

// Forward declarations to avoid circular dependencies
class Synapse;
class Neuron;

/**
 * @brief Utility class containing static helper methods
 */
class Utils {
public:
    /**
     * @brief Generates a random unique ID string
     * @param prefix Optional prefix for the ID
     * @return A unique ID string
     */
    static std::string generateUniqueId(const std::string& prefix = "");
    
    /**
     * @brief Generates a UUID (Universally Unique Identifier)
     * @return A UUID string
     */
    static std::string generateUUID();
    
    /**
     * @brief Calculate sigmoid activation function
     * @param x Input value
     * @return Sigmoid of x (between 0 and 1)
     */
    static float sigmoid(float x);
    
    /**
     * @brief Calculate tanh activation function
     * @param x Input value
     * @return Tanh of x (between -1 and 1)
     */
    static float tanh(float x);
    
    /**
     * @brief Calculate ReLU activation function
     * @param x Input value
     * @return max(0, x)
     */
    static float relu(float x);
    
    /**
     * @brief Calculate softmax for a vector of values
     * @param values Vector of input values
     * @return Vector of softmax probabilities (sums to 1)
     */
    static std::vector<float> softmax(const std::vector<float>& values);
    
    /**
     * @brief Generate a random float in a range
     * @param min Minimum value
     * @param max Maximum value
     * @return Random float between min and max
     */
    static float randomFloat(float min = 0.0f, float max = 1.0f);
    
    /**
     * @brief Generate a random integer in a range
     * @param min Minimum value
     * @param max Maximum value
     * @return Random integer between min and max (inclusive)
     */
    static int randomInt(int min, int max);
    
    /**
     * @brief Get current timestamp in milliseconds
     * @return Current timestamp in ms
     */
    static int64_t currentTimeMillis();
    
    /**
     * @brief Get current timestamp in microseconds
     * @return Current timestamp in μs
     */
    static int64_t currentTimeMicros();
    
    /**
     * @brief Create a simple hash of a string
     * @param str String to hash
     * @return Hash value as a string
     */
    static std::string simpleHash(const std::string& str);
};

/**
 * @brief A thread pool for parallel execution of neural processing
 */
class ThreadPool {
public:
    /**
     * @brief Constructor
     * @param numThreads Number of worker threads
     */
    ThreadPool(size_t numThreads);
    
    /**
     * @brief Destructor
     */
    ~ThreadPool();
    
    /**
     * @brief Add a task to the thread pool
     * @param task Function to execute
     */
    void enqueue(std::function<void()> task);
    
    /**
     * @brief Wait for all tasks to complete
     */
    void waitForCompletion();
    
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    std::mutex queueMutex;
    std::condition_variable condition;
    bool stop;
    
    int pendingTasks;
    std::mutex counterMutex;
    std::condition_variable counterCondition;
};

/**
 * @brief Memory manager for optimized allocation of neural components
 */
class MemoryManager {
public:
    /**
     * @brief Get the singleton instance
     * @return Reference to the memory manager
     */
    static MemoryManager& getInstance();
    
    /**
     * @brief Allocate memory for a synapse
     * @return Pointer to allocated memory
     */
    void* allocateSynapse();
    
    /**
     * @brief Deallocate memory used by a synapse
     * @param ptr Pointer to memory to deallocate
     */
    void deallocateSynapse(void* ptr);
    
    /**
     * @brief Get the number of active synapses
     * @return Count of active synapses
     */
    size_t getActiveSynapseCount() const;
    
    /**
     * @brief Get the total number of synapses created
     * @return Total synapse count
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Reset the memory manager statistics
     */
    void resetStats();
    
private:
    // Private constructor for singleton
    MemoryManager();
    // Prevent copying
    MemoryManager(const MemoryManager&) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;
    
    std::mutex mutex;
    size_t activeSynapses;
    size_t totalSynapses;
};

#endif // UTILS_H

