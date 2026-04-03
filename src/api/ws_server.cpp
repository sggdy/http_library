#include "cpphttp/api/ws_server.hpp"
#include "cpphttp/core/connection.hpp"
#include "cpphttp/core/codec/ws_codec.hpp"
#include "cpphttp/platform/asio_wrapper.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

namespace cpphttp {

struct WebSocketServer::Impl {
    uint16_t port = 0;
    std::unique_ptr<AsioWrapper> asio;
    boost::asio::ip::tcp::acceptor acceptor;
    boost::asio::ip::tcp::socket socket;
    std::atomic<bool> running{false};
    std::unordered_map<uint64_t, std::shared_ptr<Connection>> sessions;
    
    std::function<void(WsSession*)> onConnectCallback;
    std::function<void(WsSession*, const std::string&)> onMessageCallback;
    std::function<void(WsSession*, const std::vector<uint8_t>&)> onBinaryCallback;
    std::function<void(WsSession*, uint16_t)> onCloseCallback;
    std::function<void(WsSession*, const Error&)> onErrorCallback;
    
    Impl() : asio(std::make_unique<AsioWrapper>()),
             acceptor(asio->context()),
             socket(asio->context()) {}
};

WebSocketServer::WebSocketServer() : impl_(std::make_unique<Impl>()) {}
WebSocketServer::~WebSocketServer() { stop(); }

WebSocketServer& WebSocketServer::setNumThreads(size_t n) { return *this; }
WebSocketServer& WebSocketServer::setMaxConnections(size_t max) { return *this; }
WebSocketServer& WebSocketServer::setPort(uint16_t port) { impl_->port = port; return *this; }

WebSocketServer& WebSocketServer::onConnect(std::function<void(WsSession*)> callback) { impl_->onConnectCallback = std::move(callback); return *this; }
WebSocketServer& WebSocketServer::onMessage(std::function<void(WsSession*, const std::string&)> callback) { impl_->onMessageCallback = std::move(callback); return *this; }
WebSocketServer& WebSocketServer::onBinary(std::function<void(WsSession*, const std::vector<uint8_t>&)> callback) { impl_->onBinaryCallback = std::move(callback); return *this; }
WebSocketServer& WebSocketServer::onClose(std::function<void(WsSession*, uint16_t)> callback) { impl_->onCloseCallback = std::move(callback); return *this; }
WebSocketServer& WebSocketServer::onError(std::function<void(WsSession*, const Error&)> callback) { impl_->onErrorCallback = std::move(callback); return *this; }

void WebSocketServer::start(uint16_t port) { impl_->port = port; startAsync(port).get(); }

void WebSocketServer::stop() {
    if (!impl_->running.load()) return;
    impl_->running.store(false);
    impl_->acceptor.close();
    impl_->asio->stop();
}

std::future<void> WebSocketServer::startAsync(uint16_t port) {
    impl_->port = port;
    return std::async(std::launch::async, [this, port]() {
        auto endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);
        impl_->acceptor.open(endpoint.protocol());
        impl_->acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        impl_->acceptor.bind(endpoint);
        impl_->acceptor.listen();
        impl_->running.store(true);
        impl_->asio->run();
    });
}

std::future<void> WebSocketServer::stopAsync() {
    return std::async(std::launch::async, [this]() { stop(); });
}

void WebSocketServer::broadcast(const std::string& message) {}
void WebSocketServer::broadcast(const uint8_t* data, size_t len) {}

WebSocketServer::WebSocketServer(WebSocketServer&&) noexcept = default;
WebSocketServer& WebSocketServer::operator=(WebSocketServer&&) noexcept = default;

}
