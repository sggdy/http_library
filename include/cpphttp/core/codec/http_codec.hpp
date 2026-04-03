#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "error.hpp"

namespace cpphttp {

struct HttpParseResult {
    bool ok = false;
    std::string error;
    size_t consumed = 0;
};

class HttpCodec {
public:
    static Error encodeRequest(const HttpRequest& req, std::vector<uint8_t>& output);
    static Error encodeResponse(const HttpResponse& resp, std::vector<uint8_t>& output);

    static HttpParseResult parseRequest(const uint8_t* data, size_t len, HttpRequest& req);
    static HttpParseResult parseResponse(const uint8_t* data, size_t len, HttpResponse& resp);

    static std::string methodToString(HttpMethod method);
    static HttpMethod stringToMethod(const std::string& str);
    static std::string versionToString(HttpVersion version);
};

}
