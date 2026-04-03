#include "cpphttp/api/ws_client.hpp"

namespace cpphttp {

struct WebSocketClient::Impl {
    std::string uri;
    std::function<void()> onConnectCallback;
    std::function<void(const std::string&)> onMessageCallback;
    std::function<void(const std::vector<uint8_t>&)> onBinaryCallback;
    std::function<void(uint16_t)> onCloseCallback;
    std::function<void(const Error&)> onErrorCallback;
};

WebSocketClient::WebSocketClient() : impl_(std::make_unique<Impl>()) {}
WebSocketClient::~WebSocketClient() = default;
WebSocketClient::WebSocketClient(WebSocketClient&&) noexcept = default;
WebSocketClient& WebSocketClient::operator=(WebSocketClient&&) noexcept = default;

void WebSocketClient::connect(const std::string& uri) { impl_->uri = uri; }
std::future<void> WebSocketClient::connectAsync(const std::string& uri) { impl_->uri = uri; return std::future<void>(); }
void WebSocketClient::close(uint16_t code) {}

void WebSocketClient::send(const std::string& message) {}
void WebSocketClient::send(const uint8_t* data, size_t len) {}

WebSocketClient& WebSocketClient::onConnect(std::function<void()> callback) { impl_->onConnectCallback = std::move(callback); return *this; }
WebSocketClient& WebSocketClient::onMessage(std::function<void(const std::string&)> callback) { impl_->onMessageCallback = std::move(callback); return *this; }
WebSocketClient& WebSocketClient::onBinary(std::function<void(const std::vector<uint8_t>&)> callback) { impl_->onBinaryCallback = std::move(callback); return *this; }
WebSocketClient& WebSocketClient::onClose(std::function<void(uint16_t)> callback) { impl_->onCloseCallback = std::move(callback); return *this; }
WebSocketClient& WebSocketClient::onError(std::function<void(const Error&)> callback) { impl_->onErrorCallback = std::move(callback); return *this; }

}
