#pragma once

#include "http_types.hpp"
#include <memory>
#include <string>

namespace cpphttp {

class HttpClient {
public:
    HttpClient();
    explicit HttpClient(const std::string& baseUrl);
    ~HttpClient();

    HttpResponse get(const std::string& path);
    HttpResponse post(const std::string& path, const std::string& body);
    HttpResponse put(const std::string& path, const std::string& body);
    HttpResponse delete_(const std::string& path);

    std::future<HttpResponse> getAsync(const std::string& path);
    std::future<HttpResponse> postAsync(const std::string& path, const std::string& body);
    std::future<HttpResponse> putAsync(const std::string& path, const std::string& body);
    std::future<HttpResponse> deleteAsync(const std::string& path);

    HttpClient& setTimeout(std::chrono::seconds timeout);
    HttpClient& setKeepAlive(bool enabled);
    HttpClient& setHeader(const std::string& key, const std::string& value);
    HttpClient& setConfig(const Config& config);

    HttpClient(const HttpClient&) = delete;
    HttpClient& operator=(const HttpClient&) = delete;
    HttpClient(HttpClient&&) noexcept;
    HttpClient& operator=(HttpClient&&) noexcept;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
    void parseUrl();
    std::future<HttpResponse> requestAsync(HttpMethod method, const std::string& path, const std::string& body);
};

}
