#include "cpphttp/api/http_client.hpp"
#include "cpphttp/core/connection.hpp"
#include "cpphttp/core/codec/http_codec.hpp"
#include "cpphttp/platform/asio_wrapper.hpp"
#include <boost/asio.hpp>
#include <memory>
#include <vector>

namespace cpphttp {

struct HttpClient::Impl {
    std::string baseUrl;
    std::string host;
    uint16_t port = 80;
    Config config;
    std::unique_ptr<AsioWrapper> asio;
    
    Impl() : asio(std::make_unique<AsioWrapper>()) {}
};

HttpClient::HttpClient() : impl_(std::make_unique<Impl>()) {}
HttpClient::HttpClient(const std::string& baseUrl) : impl_(std::make_unique<Impl>()) { impl_->baseUrl = baseUrl; }
HttpClient::~HttpClient() = default;

HttpResponse HttpClient::get(const std::string& path) { return getAsync(path).get(); }
HttpResponse HttpClient::post(const std::string& path, const std::string& body) { return postAsync(path, body).get(); }
HttpResponse HttpClient::put(const std::string& path, const std::string& body) { return putAsync(path, body).get(); }
HttpResponse HttpClient::delete_(const std::string& path) { return deleteAsync(path).get(); }

std::future<HttpResponse> HttpClient::getAsync(const std::string& path) {
    std::promise<HttpResponse> promise;
    auto future = promise.get_future();
    
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(impl_->asio->context());
    boost::asio::ip::tcp::resolver resolver(impl_->asio->context());
    
    auto hostPort = impl_->baseUrl.empty() ? "localhost" : impl_->baseUrl;
    if (hostPort.find("://") != std::string::npos) {
        size_t pos = hostPort.find("://") + 3;
        hostPort = hostPort.substr(pos);
        size_t colon = hostPort.find(':');
        size_t slash = hostPort.find('/');
        if (colon != std::string::npos && (slash == std::string::npos || colon < slash)) {
            impl_->host = hostPort.substr(0, colon);
            impl_->port = static_cast<uint16_t>(std::stoi(hostPort.substr(colon + 1)));
        } else {
            impl_->host = hostPort.substr(0, slash);
        }
    }
    
    auto endpoints = resolver.resolve(impl_->host, std::to_string(impl_->port));
    
    boost::asio::async_connect(*socket, endpoints,
        [socket, this, path, promise = std::move(promise)](boost::system::error_code ec, auto) mutable {
            HttpResponse resp;
            if (ec) {
                resp.statusCode = 500;
                resp.body = ec.message();
                promise.set_value(resp);
                return;
            }
            
            HttpRequest req;
            req.method = HttpMethod::GET;
            req.path = path;
            req.version = HttpVersion::HTTP_1_1;
            req.headers = "Host: " + impl_->host + "\r\nConnection: close\r\n";
            
            std::vector<uint8_t> data;
            HttpCodec::encodeRequest(req, data);
            
            boost::asio::async_write(*socket, boost::asio::buffer(data),
                [socket, promise = std::move(promise)](boost::system::error_code ec, size_t) mutable {
                    HttpResponse resp;
                    if (ec) {
                        resp.statusCode = 500;
                        resp.body = ec.message();
                        promise.set_value(resp);
                        return;
                    }
                    
                    auto buffer = std::make_shared<std::vector<uint8_t>>(4096);
                    boost::asio::async_read(*socket, boost::asio::buffer(*buffer),
                        [buffer, promise = std::move(promise)](boost::system::error_code ec, size_t len) mutable {
                            HttpResponse resp;
                            if (!ec || ec.value() == 2) {
                                HttpCodec::parseResponse(buffer->data(), len, resp);
                            } else {
                                resp.statusCode = 500;
                                resp.body = ec.message();
                            }
                            promise.set_value(resp);
                        });
                });
        });
    
    std::thread([this]() { impl_->asio->run(); }).detach();
    return future;
}

std::future<HttpResponse> HttpClient::postAsync(const std::string& path, const std::string& body) {
    std::promise<HttpResponse> promise;
    auto future = promise.get_future();
    future.get();
    return future;
}

std::future<HttpResponse> HttpClient::putAsync(const std::string& path, const std::string& body) {
    std::promise<HttpResponse> promise;
    auto future = promise.get_future();
    future.get();
    return future;
}

std::future<HttpResponse> HttpClient::deleteAsync(const std::string& path) {
    std::promise<HttpResponse> promise;
    auto future = promise.get_future();
    future.get();
    return future;
}

HttpClient& HttpClient::setTimeout(std::chrono::seconds timeout) { impl_->config.setRequestTimeout(timeout); return *this; }
HttpClient& HttpClient::setKeepAlive(bool enabled) { impl_->config.setKeepAlive(enabled); return *this; }
HttpClient& HttpClient::setHeader(const std::string& key, const std::string& value) { return *this; }
HttpClient& HttpClient::setConfig(const Config& config) { impl_->config = config; return *this; }

HttpClient::HttpClient(HttpClient&&) noexcept = default;
HttpClient& HttpClient::operator=(HttpClient&&) noexcept = default;

}
