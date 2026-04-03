#include "cpphttp/api/http_client.hpp"
#include "cpphttp/core/codec/http_codec.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include <thread>

namespace cpphttp {

struct HttpClient::Impl {
    std::string baseUrl;
    std::string host;
    uint16_t port = 80;
    Config config;
    
    Impl() : config() {
        config.setRequestTimeout(std::chrono::seconds(10));
    }
};

HttpClient::HttpClient() : impl_(std::make_unique<Impl>()) {}
HttpClient::HttpClient(const std::string& baseUrl) : impl_(std::make_unique<Impl>()) { impl_->baseUrl = baseUrl; }
HttpClient::~HttpClient() = default;

void HttpClient::parseUrl() {
    auto hostPort = impl_->baseUrl.empty() ? "localhost" : impl_->baseUrl;
    if (hostPort.find("://") != std::string::npos) {
        size_t pos = hostPort.find("://") + 3;
        hostPort = hostPort.substr(pos);
    }
    size_t slash = hostPort.find('/');
    if (slash != std::string::npos) {
        hostPort = hostPort.substr(0, slash);
    }
    size_t colon = hostPort.find(':');
    if (colon != std::string::npos) {
        impl_->host = hostPort.substr(0, colon);
        impl_->port = static_cast<uint16_t>(std::stoi(hostPort.substr(colon + 1)));
    } else {
        impl_->host = hostPort;
    }
}

HttpResponse HttpClient::get(const std::string& path) {
    auto future = getAsync(path);
    if (!future.valid()) return HttpResponse{};
    
    auto timeout = impl_->config.getRequestTimeout();
    auto status = future.wait_for(timeout);
    if (status == std::future_status::timeout) {
        HttpResponse resp;
        resp.statusCode = 408;
        resp.body = "Request Timeout";
        return resp;
    }
    return future.get();
}

HttpResponse HttpClient::post(const std::string& path, const std::string& body) {
    auto future = postAsync(path, body);
    if (!future.valid()) return HttpResponse{};
    auto timeout = impl_->config.getRequestTimeout();
    auto status = future.wait_for(timeout);
    if (status == std::future_status::timeout) {
        HttpResponse resp;
        resp.statusCode = 408;
        resp.body = "Request Timeout";
        return resp;
    }
    return future.get();
}

HttpResponse HttpClient::put(const std::string& path, const std::string& body) {
    auto future = putAsync(path, body);
    if (!future.valid()) return HttpResponse{};
    auto timeout = impl_->config.getRequestTimeout();
    auto status = future.wait_for(timeout);
    if (status == std::future_status::timeout) {
        HttpResponse resp;
        resp.statusCode = 408;
        resp.body = "Request Timeout";
        return resp;
    }
    return future.get();
}

HttpResponse HttpClient::delete_(const std::string& path) {
    auto future = deleteAsync(path);
    if (!future.valid()) return HttpResponse{};
    auto timeout = impl_->config.getRequestTimeout();
    auto status = future.wait_for(timeout);
    if (status == std::future_status::timeout) {
        HttpResponse resp;
        resp.statusCode = 408;
        resp.body = "Request Timeout";
        return resp;
    }
    return future.get();
}

std::future<HttpResponse> HttpClient::getAsync(const std::string& path) {
    return requestAsync(HttpMethod::GET, path, "");
}

std::future<HttpResponse> HttpClient::postAsync(const std::string& path, const std::string& body) {
    return requestAsync(HttpMethod::POST, path, body);
}

std::future<HttpResponse> HttpClient::putAsync(const std::string& path, const std::string& body) {
    return requestAsync(HttpMethod::PUT, path, body);
}

std::future<HttpResponse> HttpClient::deleteAsync(const std::string& path) {
    return requestAsync(HttpMethod::DELETE, path, "");
}

std::future<HttpResponse> HttpClient::requestAsync(HttpMethod method, const std::string& path, const std::string& body) {
    std::promise<HttpResponse> promise;
    auto future = promise.get_future();
    
    if (impl_->baseUrl.empty()) {
        impl_->host = "127.0.0.1";
        impl_->port = 80;
    } else {
        parseUrl();
    }
    
    std::thread([this, method, path, body, promise = std::move(promise)]() mutable {
        boost::asio::io_context ctx;
        boost::asio::ip::tcp::socket socket(ctx);
        boost::asio::ip::tcp::resolver resolver(ctx);
        
        try {
            auto endpoints = resolver.resolve(impl_->host, std::to_string(impl_->port));
            boost::asio::connect(socket, endpoints);
            
            HttpRequest req;
            req.method = method;
            req.path = path;
            req.version = HttpVersion::HTTP_1_1;
            req.body = body;
            req.headers = "Host: " + impl_->host + "\r\n";
            if (!body.empty()) {
                req.headers += "Content-Length: " + std::to_string(body.size()) + "\r\n";
            }
            req.headers += "Connection: close\r\n";
            
            std::vector<uint8_t> data;
            HttpCodec::encodeRequest(req, data);
            boost::asio::write(socket, boost::asio::buffer(data));
            
            std::vector<uint8_t> buffer(8192);
            boost::system::error_code ec;
            size_t len = socket.read_some(boost::asio::buffer(buffer), ec);
            
            HttpResponse resp;
            if (len > 0) {
                HttpCodec::parseResponse(buffer.data(), len, resp);
            } else {
                resp.statusCode = 500;
                resp.body = ec.message();
            }
            promise.set_value(resp);
            
        } catch (const boost::system::system_error& e) {
            HttpResponse resp;
            resp.statusCode = 500;
            resp.statusMessage = e.what();
            promise.set_value(resp);
        }
    }).detach();
    
    return future;
}

HttpClient& HttpClient::setTimeout(std::chrono::seconds timeout) { 
    impl_->config.setRequestTimeout(timeout); 
    return *this; 
}

HttpClient& HttpClient::setKeepAlive(bool enabled) { impl_->config.setKeepAlive(enabled); return *this; }
HttpClient& HttpClient::setHeader(const std::string& key, const std::string& value) { return *this; }
HttpClient& HttpClient::setConfig(const Config& config) { impl_->config = config; return *this; }

HttpClient::HttpClient(HttpClient&&) noexcept = default;
HttpClient& HttpClient::operator=(HttpClient&&) noexcept = default;

}
