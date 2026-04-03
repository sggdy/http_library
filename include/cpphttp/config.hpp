#pragma once

#include <chrono>
#include <cstddef>

namespace cpphttp {

class Config {
public:
    Config& setNumThreads(size_t n) { numThreads_ = n; return *this; }
    Config& setMaxConnections(size_t max) { maxConnections_ = max; return *this; }
    Config& setRequestTimeout(std::chrono::seconds timeout) { requestTimeout_ = timeout; return *this; }
    Config& setKeepAlive(bool enabled) { keepAlive_ = enabled; return *this; }
    Config& setKeepAliveTimeout(std::chrono::seconds timeout) { keepAliveTimeout_ = timeout; return *this; }
    Config& setSendBufferSize(size_t size) { sendBufferSize_ = size; return *this; }
    Config& setRecvBufferSize(size_t size) { recvBufferSize_ = size; return *this; }

    size_t getNumThreads() const { return numThreads_; }
    size_t getMaxConnections() const { return maxConnections_; }
    std::chrono::seconds getRequestTimeout() const { return requestTimeout_; }
    bool getKeepAlive() const { return keepAlive_; }
    std::chrono::seconds getKeepAliveTimeout() const { return keepAliveTimeout_; }
    size_t getSendBufferSize() const { return sendBufferSize_; }
    size_t getRecvBufferSize() const { return recvBufferSize_; }

private:
    size_t numThreads_ = 4;
    size_t maxConnections_ = 100000;
    std::chrono::seconds requestTimeout_{30};
    bool keepAlive_ = true;
    std::chrono::seconds keepAliveTimeout_{60};
    size_t sendBufferSize_ = 64 * 1024;
    size_t recvBufferSize_ = 64 * 1024;
};

}
