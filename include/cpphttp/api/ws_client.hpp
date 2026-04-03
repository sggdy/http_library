#pragma once

#include "buffer.hpp"
#include <cstdint>
#include <functional>
#include <future>
#include <memory>
#include <string>
#include <vector>

namespace cpphttp {

class WebSocketClient {
public:
    WebSocketClient();
    ~WebSocketClient();

    void connect(const std::string& uri);
    std::future<void> connectAsync(const std::string& uri);
    void close(uint16_t code = 1000);

    void send(const std::string& message);
    void send(const uint8_t* data, size_t len);

    WebSocketClient& onConnect(std::function<void()> callback);
    WebSocketClient& onMessage(std::function<void(const std::string&)> callback);
    WebSocketClient& onBinary(std::function<void(const std::vector<uint8_t>&)> callback);
    WebSocketClient& onClose(std::function<void(uint16_t)> callback);
    WebSocketClient& onError(std::function<void(const Error&)> callback);

    WebSocketClient(const WebSocketClient&) = delete;
    WebSocketClient& operator=(const WebSocketClient&) = delete;
    WebSocketClient(WebSocketClient&&) noexcept;
    WebSocketClient& operator=(WebSocketClient&&) noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
