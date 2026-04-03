#include "cpphttp/api/http_server.hpp"
#include "cpphttp/core/router.hpp"
#include "cpphttp/core/codec/http_codec.hpp"
#include "cpphttp/platform/asio_wrapper.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <atomic>

namespace cpphttp {

struct HttpServer::Impl {
    Config config;
    std::unique_ptr<AsioWrapper> asio;
    std::unique_ptr<Router> router;
    boost::asio::ip::tcp::acceptor acceptor;
    std::atomic<bool> running{false};
    
    Impl() : asio(std::make_unique<AsioWrapper>()), 
             router(std::make_unique<Router>()),
             acceptor(asio->context()) {}
};

HttpServer::HttpServer() : impl_(std::make_unique<Impl>()) {}
HttpServer::~HttpServer() { stop(); }

HttpServer& HttpServer::route(HttpMethod method, const std::string& path, Handler handler) {
    impl_->router->addRoute(method, path, handler);
    return *this;
}

HttpServer& HttpServer::get(const std::string& path, Handler handler) {
    return route(HttpMethod::GET, path, handler);
}

HttpServer& HttpServer::post(const std::string& path, Handler handler) {
    return route(HttpMethod::POST, path, handler);
}

HttpServer& HttpServer::put(const std::string& path, Handler handler) {
    return route(HttpMethod::PUT, path, handler);
}

HttpServer& HttpServer::delete_(const std::string& path, Handler handler) {
    return route(HttpMethod::DELETE, path, handler);
}

HttpServer& HttpServer::setNumThreads(size_t n) { return *this; }
HttpServer& HttpServer::setMaxConnections(size_t max) { return *this; }
HttpServer& HttpServer::setRequestTimeout(std::chrono::seconds timeout) { return *this; }
HttpServer& HttpServer::setConfig(const Config& config) { impl_->config = config; return *this; }

HttpServer& HttpServer::onWsConnect(std::function<void(WsSession*)> callback) { return *this; }

void HttpServer::start(const std::string& host, uint16_t port) {
    startAsync(host, port).get();
}

void HttpServer::stop() {
    if (!impl_->running.load()) return;
    impl_->running.store(false);
    impl_->acceptor.close();
    impl_->asio->stop();
}

std::future<void> HttpServer::startAsync(const std::string& host, uint16_t port) {
    return std::async(std::launch::async, [this, host, port]() {
        auto endpoint = boost::asio::ip::tcp::endpoint(
            boost::asio::ip::make_address(host), port);
        
        impl_->acceptor.open(endpoint.protocol());
        impl_->acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        impl_->acceptor.bind(endpoint);
        impl_->acceptor.listen();
        
        impl_->running.store(true);
        
        while (impl_->running.load()) {
            boost::asio::ip::tcp::socket socket(impl_->asio->context());
            boost::system::error_code ec;
            impl_->acceptor.accept(socket, ec);
            
            if (!ec) {
                std::thread([this, sock = std::move(socket)]() mutable {
                    handleConnection(std::move(sock));
                }).detach();
            }
        }
    });
}

std::future<void> HttpServer::stopAsync() {
    return std::async(std::launch::async, [this]() { stop(); });
}

void HttpServer::handleConnection(boost::asio::ip::tcp::socket socket) {
    std::vector<uint8_t> buffer(8192);
    boost::system::error_code ec;
    size_t len = socket.read_some(boost::asio::buffer(buffer), ec);
    
    if (ec || len == 0) return;
    
    HttpRequest req;
    HttpResponse resp;
    resp.setContentType("text/plain");
    
    auto result = HttpCodec::parseRequest(buffer.data(), len, req);
    if (!result.ok) {
        resp.statusCode = 400;
        resp.statusMessage = "Bad Request";
        resp.body = result.error;
    } else {
        auto handler = impl_->router->findHandler(req.method, req.path);
        if (handler) {
            handler(req, resp);
        } else {
            resp.statusCode = 404;
            resp.statusMessage = "Not Found";
            resp.body = "Not Found";
        }
    }
    
    std::vector<uint8_t> output;
    HttpCodec::encodeResponse(resp, output);
    boost::asio::write(socket, boost::asio::buffer(output), ec);
    socket.close(ec);
}

HttpServer::HttpServer(HttpServer&&) noexcept = default;
HttpServer& HttpServer::operator=(HttpServer&&) noexcept = default;

}
