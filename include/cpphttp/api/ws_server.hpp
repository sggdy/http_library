#pragma once

#include "buffer.hpp"
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <vector>

namespace cpphttp {

class WsSession {
public:
    virtual ~WsSession() = default;
    virtual uint64_t id() const = 0;
    virtual std::string remoteAddr() const = 0;
    virtual uint16_t remotePort() const = 0;
    virtual void send(const std::string& message) = 0;
    virtual void send(const uint8_t* data, size_t len) = 0;
    virtual void close(uint16_t code = 1000) = 0;
    virtual void keepAlive() = 0;
};

class WebSocketServer {
public:
    WebSocketServer();
    ~WebSocketServer();

    WebSocketServer& setNumThreads(size_t n);
    WebSocketServer& setMaxConnections(size_t max);
    WebSocketServer& setPort(uint16_t port);

    WebSocketServer& onConnect(std::function<void(WsSession*)> callback);
    WebSocketServer& onMessage(std::function<void(WsSession*, const std::string&)> callback);
    WebSocketServer& onBinary(std::function<void(WsSession*, const std::vector<uint8_t>&)> callback);
    WebSocketServer& onClose(std::function<void(WsSession*, uint16_t)> callback);
    WebSocketServer& onError(std::function<void(WsSession*, const Error&)> callback);

    void start(uint16_t port);
    void stop();
    std::future<void> startAsync(uint16_t port);
    std::future<void> stopAsync();

    void broadcast(const std::string& message);
    void broadcast(const uint8_t* data, size_t len);

    WebSocketServer(const WebSocketServer&) = delete;
    WebSocketServer& operator=(const WebSocketServer&) = delete;
    WebSocketServer(WebSocketServer&&) noexcept;
    WebSocketServer& operator=(WebSocketServer&&) noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
