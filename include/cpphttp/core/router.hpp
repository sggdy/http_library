#pragma once

#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <regex>
#include "api/http_types.hpp"

namespace cpphttp {

class Router {
public:
    struct Route {
        std::string pattern;
        std::regex regex;
        HttpMethod method;
        Handler handler;
        int priority;
    };

    Router();

    void addRoute(HttpMethod method, const std::string& pattern, Handler handler);
    Handler findHandler(HttpMethod method, const std::string& path) const;

    static std::string normalizePattern(const std::string& pattern);

private:
    std::vector<Route> routes_;
    std::unordered_map<std::string, Handler> exactMatch_;
};

inline Router::Router() = default;

inline std::string Router::normalizePattern(const std::string& pattern) {
    if (pattern.empty() || pattern[0] != '/') {
        return '/' + pattern;
    }
    return pattern;
}

}
