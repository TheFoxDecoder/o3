/**
 * @file utils.cpp
 * @brief Implementation of utility functions and classes for the Ozone (O3) architecture.
 */

#include "../include/utils.h"
#include <random>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <ctime>

// ============== Utility Functions ==============

std::string Utils::generateUniqueId(const std::string& prefix) {
    // Use a high-resolution clock to get the current time
    auto now = std::chrono::high_resolution_clock::now();
    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(
        now.time_since_epoch()).count();
    
    // Use a random device to get a random number
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 9999);
    int random = dist(gen);
    
    // Combine prefix, timestamp, and random number
    std::stringstream ss;
    ss << prefix
       << std::hex << std::setw(16) << std::setfill('0') << nanoseconds
       << std::setw(4) << random;
    
    return ss.str();
}

std::string Utils::generateUUID() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
    
    uint32_t a = dist(gen);
    uint32_t b = dist(gen);
    uint32_t c = dist(gen);
    uint32_t d = dist(gen);
    
    // Format as standard UUID (8-4-4-4-12 hexadecimal digits)
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    ss << std::setw(8) << (a) << "-";
    ss << std::setw(4) << ((b >> 16) & 0xFFFF) << "-";
    ss << std::setw(4) << (b & 0xFFFF) << "-";
    ss << std::setw(4) << ((c >> 16) & 0xFFFF) << "-";
    ss << std::setw(4) << (c & 0xFFFF);
    ss << std::setw(8) << d;
    
    return ss.str();
}

float Utils::sigmoid(float x) {
    return 1.0f / (1.0f + std::exp(-x));
}

float Utils::tanh(float x) {
    return std::tanh(x);
}

float Utils::relu(float x) {
    return std::max(0.0f, x);
}

std::vector<float> Utils::softmax(const std::vector<float>& values) {
    if (values.empty()) return {};
    
    // Find the maximum value to prevent overflow
    float maxVal = *std::max_element(values.begin(), values.end());
    
    // Compute exp of each value (shifted by max for stability)
    std::vector<float> expValues;
    float expSum = 0.0f;
    
    for (float val : values) {
        float expVal = std::exp(val - maxVal);
        expValues.push_back(expVal);
        expSum += expVal;
    }
    
    // Normalize by sum to get probabilities
    std::vector<float> result;
    for (float expVal : expValues) {
        result.push_back(expVal / expSum);
    }
    
    return result;
}

float Utils::randomFloat(float min, float max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(gen);
}

int Utils::randomInt(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(gen);
}

int64_t Utils::currentTimeMillis() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
}

int64_t Utils::currentTimeMicros() {
    auto now = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(
        now.time_since_epoch()).count();
}

std::string Utils::simpleHash(const std::string& str) {
    // Simple FNV-1a hash implementation
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET_BASIS = 14695981039346656037ULL;
    
    uint64_t hash = FNV_OFFSET_BASIS;
    for (char c : str) {
        hash ^= static_cast<uint64_t>(c);
        hash *= FNV_PRIME;
    }
    
    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << hash;
    return ss.str();
}

// ============== ThreadPool Implementation ==============

ThreadPool::ThreadPool(size_t numThreads) : stop(false), pendingTasks(0) {
    for (size_t i = 0; i < numThreads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    });
                    
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }
                    
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                
                // Execute the task
                task();
                
                // Decrement pending tasks counter
                {
                    std::lock_guard<std::mutex> lock(this->counterMutex);
                    --this->pendingTasks;
                }
                
                // Notify waitForCompletion
                this->counterCondition.notify_all();
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        stop = true;
    }
    
    condition.notify_all();
    
    for (std::thread& worker : workers) {
        worker.join();
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        if (stop) {
            throw std::runtime_error("ThreadPool is stopped");
        }
        
        tasks.emplace(std::move(task));
        
        // Increment pending tasks counter
        {
            std::lock_guard<std::mutex> counterLock(counterMutex);
            ++pendingTasks;
        }
    }
    
    condition.notify_one();
}

void ThreadPool::waitForCompletion() {
    std::unique_lock<std::mutex> lock(counterMutex);
    counterCondition.wait(lock, [this] {
        return pendingTasks == 0;
    });
}

// ============== MemoryManager Implementation ==============

MemoryManager& MemoryManager::getInstance() {
    static MemoryManager instance;
    return instance;
}

MemoryManager::MemoryManager() : activeSynapses(0), totalSynapses(0) {
}

void* MemoryManager::allocateSynapse() {
    std::lock_guard<std::mutex> lock(mutex);
    
    // In a more sophisticated implementation, this would use
    // a memory pool or slab allocator for efficiency
    void* ptr = ::operator new(sizeof(void*) * 16);  // Approximate Synapse size
    
    ++activeSynapses;
    ++totalSynapses;
    
    return ptr;
}

void MemoryManager::deallocateSynapse(void* ptr) {
    if (!ptr) return;
    
    std::lock_guard<std::mutex> lock(mutex);
    
    ::operator delete(ptr);
    
    if (activeSynapses > 0) {
        --activeSynapses;
    }
}

size_t MemoryManager::getActiveSynapseCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return activeSynapses;
}

size_t MemoryManager::getTotalSynapseCount() const {
    std::lock_guard<std::mutex> lock(mutex);
    return totalSynapses;
}

void MemoryManager::resetStats() {
    std::lock_guard<std::mutex> lock(mutex);
    activeSynapses = 0;
    totalSynapses = 0;
}

