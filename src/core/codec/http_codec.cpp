#include "core/codec/http_codec.hpp"
#include "api/http_types.hpp"
#include <algorithm>

namespace cpphttp {

std::string HttpCodec::methodToString(HttpMethod method) {
    switch (method) {
        case HttpMethod::GET: return "GET";
        case HttpMethod::POST: return "POST";
        case HttpMethod::PUT: return "PUT";
        case HttpMethod::DELETE: return "DELETE";
        case HttpMethod::PATCH: return "PATCH";
        case HttpMethod::HEAD: return "HEAD";
        case HttpMethod::OPTIONS: return "OPTIONS";
        case HttpMethod::CONNECT: return "CONNECT";
        case HttpMethod::TRACE: return "TRACE";
        default: return "GET";
    }
}

HttpMethod HttpCodec::stringToMethod(const std::string& str) {
    if (str == "GET") return HttpMethod::GET;
    if (str == "POST") return HttpMethod::POST;
    if (str == "PUT") return HttpMethod::PUT;
    if (str == "DELETE") return HttpMethod::DELETE;
    if (str == "PATCH") return HttpMethod::PATCH;
    if (str == "HEAD") return HttpMethod::HEAD;
    if (str == "OPTIONS") return HttpMethod::OPTIONS;
    if (str == "CONNECT") return HttpMethod::CONNECT;
    if (str == "TRACE") return HttpMethod::TRACE;
    return HttpMethod::GET;
}

std::string HttpCodec::versionToString(HttpVersion version) {
    switch (version) {
        case HttpVersion::HTTP_1_0: return "HTTP/1.0";
        case HttpVersion::HTTP_1_1: return "HTTP/1.1";
        case HttpVersion::HTTP_2_0: return "HTTP/2.0";
        default: return "HTTP/1.1";
    }
}

Error HttpCodec::encodeRequest(const HttpRequest& req, std::vector<uint8_t>& output) {
    std::string line = methodToString(req.method) + " " + req.path;
    if (!req.query.empty()) line += "?" + req.query;
    line += " " + versionToString(req.version) + "\r\n";
    
    output.insert(output.end(), line.begin(), line.end());
    output.insert(output.end(), req.headers.begin(), req.headers.end());
    output.insert(output.end(), req.body.begin(), req.body.end());
    return Error::success();
}

Error HttpCodec::encodeResponse(const HttpResponse& resp, std::vector<uint8_t>& output) {
    std::string line = versionToString(resp.version) + " " + std::to_string(resp.statusCode) 
                     + " " + resp.statusMessage + "\r\n";
    output.insert(output.end(), line.begin(), line.end());
    output.insert(output.end(), resp.headers.begin(), resp.headers.end());
    output.insert(output.end(), resp.body.begin(), resp.body.end());
    return Error::success();
}

HttpParseResult HttpCodec::parseRequest(const uint8_t* data, size_t len, HttpRequest& req) {
    HttpParseResult result;
    if (len < 4) {
        result.error = "Incomplete request";
        return result;
    }

    auto view = std::string_view(reinterpret_cast<const char*>(data), len);
    auto pos = view.find("\r\n");
    if (pos == std::string_view::npos) {
        result.error = "Missing request line terminator";
        return result;
    }

    auto line = view.substr(0, pos);
    auto words = std::vector<std::string>();
    size_t start = 0;
    while (start < line.size()) {
        auto ws = line.find(' ', start);
        if (ws == std::string_view::npos) {
            words.emplace_back(line.substr(start));
            break;
        }
        words.emplace_back(line.substr(start, ws - start));
        start = ws + 1;
        while (start < line.size() && line[start] == ' ') start++;
    }

    if (words.size() < 3) {
        result.error = "Invalid request line";
        return result;
    }

    req.method = stringToMethod(words[0]);
    auto pathAndQuery = words[1];
    auto qpos = pathAndQuery.find('?');
    if (qpos != std::string_view::npos) {
        req.path = std::string(pathAndQuery.substr(0, qpos));
        req.query = std::string(pathAndQuery.substr(qpos + 1));
    } else {
        req.path = std::string(pathAndQuery);
    }
    
    size_t headerEnd = view.find("\r\n\r\n");
    if (headerEnd != std::string_view::npos) {
        req.headers = std::string(view.substr(pos + 2, headerEnd - pos - 2));
        auto bodyStart = headerEnd + 4;
        if (bodyStart < len) {
            req.body = std::string(view.substr(bodyStart));
        }
    }

    result.ok = true;
    result.consumed = len;
    return result;
}

HttpParseResult HttpCodec::parseResponse(const uint8_t* data, size_t len, HttpResponse& resp) {
    HttpParseResult result;
    if (len < 4) {
        result.error = "Incomplete response";
        return result;
    }

    auto view = std::string_view(reinterpret_cast<const char*>(data), len);
    auto pos = view.find("\r\n");
    if (pos == std::string_view::npos) {
        result.error = "Missing status line terminator";
        return result;
    }

    auto line = view.substr(0, pos);
    auto words = std::vector<std::string>();
    size_t start = 0;
    while (start < line.size()) {
        auto ws = line.find(' ', start);
        if (ws == std::string_view::npos) {
            words.emplace_back(line.substr(start));
            break;
        }
        words.emplace_back(line.substr(start, ws - start));
        start = ws + 1;
    }

    if (words.size() < 2) {
        result.error = "Invalid status line";
        return result;
    }

    resp.version = (words[0] == "HTTP/1.0") ? HttpVersion::HTTP_1_0 : HttpVersion::HTTP_1_1;
    resp.statusCode = static_cast<uint16_t>(std::stoi(words[1]));
    if (words.size() > 2) {
        resp.statusMessage = std::string(words[2]);
    }

    size_t headerEnd = view.find("\r\n\r\n");
    if (headerEnd != std::string_view::npos) {
        resp.headers = std::string(view.substr(pos + 2, headerEnd - pos - 2));
        auto bodyStart = headerEnd + 4;
        if (bodyStart < len) {
            resp.body = std::string(view.substr(bodyStart));
        }
    }

    result.ok = true;
    result.consumed = len;
    return result;
}

}
