#include "api/ws_server.hpp"

namespace cpphttp {

struct WebSocketServer::Impl {
    uint16_t port = 0;
    size_t numThreads = 4;
    size_t maxConnections = 100000;
    std::function<void(WsSession*)> onConnectCallback;
    std::function<void(WsSession*, const std::string&)> onMessageCallback;
    std::function<void(WsSession*, const std::vector<uint8_t>&)> onBinaryCallback;
    std::function<void(WsSession*, uint16_t)> onCloseCallback;
    std::function<void(WsSession*, const Error&)> onErrorCallback;
};

WebSocketServer::WebSocketServer() : impl_(std::make_unique<Impl>()) {}
WebSocketServer::~WebSocketServer() = default;
WebSocketServer::WebSocketServer(WebSocketServer&&) noexcept = default;
WebSocketServer& WebSocketServer::operator=(WebSocketServer&&) noexcept = default;

WebSocketServer& WebSocketServer::setNumThreads(size_t n) { impl_->numThreads = n; return *this; }
WebSocketServer& WebSocketServer::setMaxConnections(size_t max) { impl_->maxConnections = max; return *this; }
WebSocketServer& WebSocketServer::setPort(uint16_t port) { impl_->port = port; return *this; }

WebSocketServer& WebSocketServer::onConnect(std::function<void(WsSession*)> callback) { impl_->onConnectCallback = std::move(callback); return *this; }
WebSocketServer& WebSocketServer::onMessage(std::function<void(WsSession*, const std::string&)> callback) { impl_->onMessageCallback = std::move(callback); return *this; }
WebSocketServer& WebSocketServer::onBinary(std::function<void(WsSession*, const std::vector<uint8_t>&)> callback) { impl_->onBinaryCallback = std::move(callback); return *this; }
WebSocketServer& WebSocketServer::onClose(std::function<void(WsSession*, uint16_t)> callback) { impl_->onCloseCallback = std::move(callback); return *this; }
WebSocketServer& WebSocketServer::onError(std::function<void(WsSession*, const Error&)> callback) { impl_->onErrorCallback = std::move(callback); return *this; }

void WebSocketServer::start(uint16_t port) { impl_->port = port; }
void WebSocketServer::stop() {}
std::future<void> WebSocketServer::startAsync(uint16_t port) { impl_->port = port; return std::future<void>(); }
std::future<void> WebSocketServer::stopAsync() { return std::future<void>(); }

void WebSocketServer::broadcast(const std::string& message) {}
void WebSocketServer::broadcast(const uint8_t* data, size_t len) {}

}
