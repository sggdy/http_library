#include "api/http_client.hpp"

namespace cpphttp {

struct HttpClient::Impl {
    std::string baseUrl;
    Config config;
};

HttpClient::HttpClient() : impl_(std::make_unique<Impl>()) {}
HttpClient::HttpClient(const std::string& baseUrl) : impl_(std::make_unique<Impl>()) { impl_->baseUrl = baseUrl; }
HttpClient::~HttpClient() = default;
HttpClient::HttpClient(HttpClient&&) noexcept = default;
HttpClient& HttpClient::operator=(HttpClient&&) noexcept = default;

HttpResponse HttpClient::get(const std::string& path) { return HttpResponse(); }
HttpResponse HttpClient::post(const std::string& path, const std::string& body) { return HttpResponse(); }
HttpResponse HttpClient::put(const std::string& path, const std::string& body) { return HttpResponse(); }
HttpResponse HttpClient::delete_(const std::string& path) { return HttpResponse(); }

std::future<HttpResponse> HttpClient::getAsync(const std::string& path) { return std::future<HttpResponse>(); }
std::future<HttpResponse> HttpClient::postAsync(const std::string& path, const std::string& body) { return std::future<HttpResponse>(); }
std::future<HttpResponse> HttpClient::putAsync(const std::string& path, const std::string& body) { return std::future<HttpResponse>(); }
std::future<HttpResponse> HttpClient::deleteAsync(const std::string& path) { return std::future<HttpResponse>(); }

HttpClient& HttpClient::setTimeout(std::chrono::seconds timeout) { impl_->config.setRequestTimeout(timeout); return *this; }
HttpClient& HttpClient::setKeepAlive(bool enabled) { impl_->config.setKeepAlive(enabled); return *this; }
HttpClient& HttpClient::setHeader(const std::string& key, const std::string& value) { return *this; }
HttpClient& HttpClient::setConfig(const Config& config) { impl_->config = config; return *this; }

}
