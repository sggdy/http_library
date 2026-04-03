#pragma once

#include "buffer.hpp"
#include "config.hpp"
#include <functional>
#include <future>
#include <string>
#include <cstdint>

namespace cpphttp {

enum class HttpMethod {
    GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS, CONNECT, TRACE
};

enum class HttpVersion {
    HTTP_1_0, HTTP_1_1, HTTP_2_0
};

struct HttpRequest {
    HttpMethod method;
    std::string path;
    std::string query;
    HttpVersion version;
    std::string headers;
    std::string body;
    std::string remoteAddr;
    uint16_t remotePort = 0;
};

struct HttpResponse {
    HttpVersion version = HttpVersion::HTTP_1_1;
    uint16_t statusCode = 200;
    std::string statusMessage{"OK"};
    std::string headers;
    std::string body;

    HttpResponse& setContentType(const std::string& type);
    HttpResponse& setKeepAlive(bool enabled);
    HttpResponse& setBody(const std::string& b);
    HttpResponse& setHeader(const std::string& key, const std::string& value);
};

using Handler = std::function<void(const HttpRequest&, HttpResponse&)>;

}
