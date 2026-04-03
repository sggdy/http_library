#include "cpphttp/api/http_types.hpp"

namespace cpphttp {

HttpResponse& HttpResponse::setContentType(const std::string& type) {
    if (!headers.empty() && headers.back() != '\n') {
        headers += "\r\n";
    }
    headers += "Content-Type: " + type + "\r\n";
    return *this;
}

HttpResponse& HttpResponse::setKeepAlive(bool enabled) {
    if (!headers.empty() && headers.back() != '\n') {
        headers += "\r\n";
    }
    if (enabled) {
        headers += "Connection: keep-alive\r\n";
    } else {
        headers += "Connection: close\r\n";
    }
    return *this;
}

HttpResponse& HttpResponse::setBody(const std::string& b) {
    body = b;
    if (!headers.empty() && headers.back() != '\n') {
        headers += "\r\n";
    }
    headers += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    return *this;
}

HttpResponse& HttpResponse::setHeader(const std::string& key, const std::string& value) {
    if (!headers.empty() && headers.back() != '\n') {
        headers += "\r\n";
    }
    headers += key + ": " + value + "\r\n";
    return *this;
}

}
