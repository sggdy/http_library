#include "cpphttp/api/http_server.hpp"

namespace cpphttp {

struct HttpServer::Impl {
    Config config;
};

HttpServer::HttpServer() : impl_(std::make_unique<Impl>()) {}
HttpServer::~HttpServer() = default;
HttpServer::HttpServer(HttpServer&&) noexcept = default;
HttpServer& HttpServer::operator=(HttpServer&&) noexcept = default;

HttpServer& HttpServer::route(HttpMethod method, const std::string& path, Handler handler) {
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

HttpServer& HttpServer::setNumThreads(size_t n) { impl_->config.setNumThreads(n); return *this; }
HttpServer& HttpServer::setMaxConnections(size_t max) { impl_->config.setMaxConnections(max); return *this; }
HttpServer& HttpServer::setRequestTimeout(std::chrono::seconds timeout) { impl_->config.setRequestTimeout(timeout); return *this; }
HttpServer& HttpServer::setConfig(const Config& config) { impl_->config = config; return *this; }

void HttpServer::start(const std::string& host, uint16_t port) {}
void HttpServer::stop() {}
std::future<void> HttpServer::startAsync(const std::string& host, uint16_t port) { return std::future<void>(); }
std::future<void> HttpServer::stopAsync() { return std::future<void>(); }

}
