#include "core/router.hpp"
#include <algorithm>

namespace cpphttp {

void Router::addRoute(HttpMethod method, const std::string& pattern, Handler handler) {
    Route route;
    route.pattern = normalizePattern(pattern);
    route.method = method;
    route.handler = std::move(handler);
    route.priority = 0;

    std::string regexPattern = route.pattern;
    regexPattern = std::regex_replace(regexPattern, std::regex("\\/"), "\\/");
    regexPattern = std::regex_replace(regexPattern, std::regex(":([^/]+)"), "([^/]+)");
    regexPattern = std::regex_replace(regexPattern, std::regex("\\*"), ".*");

    try {
        route.regex = std::regex(regexPattern);
    } catch (const std::regex_error&) {
        route.regex = std::regex(route.pattern);
        route.priority = 1;
    }

    routes_.push_back(std::move(route));
    std::stable_sort(routes_.begin(), routes_.end(),
        [](const Route& a, const Route& b) { return a.priority > b.priority; });
}

Handler Router::findHandler(HttpMethod method, const std::string& path) const {
    auto it = exactMatch_.find(path);
    if (it != exactMatch_.end()) {
        return it->second;
    }

    for (const auto& route : routes_) {
        if (route.method != method) continue;
        if (std::regex_match(path, route.regex)) {
            return route.handler;
        }
    }

    return nullptr;
}

}
