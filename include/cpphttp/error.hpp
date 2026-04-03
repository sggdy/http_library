#pragma once

#include <cstdint>
#include <string>
#include <system_error>

namespace cpphttp {

enum class ErrorCode : uint32_t {
    Success = 0x0000,
    Unknown = 0x0001,
    OutOfMemory = 0x0002,

    NetworkError = 0x0100,
    ConnectionRefused = 0x0101,
    ConnectionTimeout = 0x0102,
    ConnectionReset = 0x0103,
    HostUnreachable = 0x0104,
    BrokenPipe = 0x0105,

    HttpError = 0x0200,
    BadRequest = 0x0201,
    NotFound = 0x0202,
    MethodNotAllowed = 0x0203,
    InternalServerError = 0x0204,

    WsError = 0x0300,
    WsHandshakeFailed = 0x0301,
    WsInvalidFrame = 0x0302,
    WsProtocolError = 0x0303,
    WsMaskingError = 0x0304,
};

struct Error {
    ErrorCode code;
    std::string message;
    std::string details;

    Error() : code(ErrorCode::Success), message("Success") {}
    Error(ErrorCode c, const std::string& msg, const std::string& det = "")
        : code(c), message(msg), details(det) {}

    bool ok() const { return code == ErrorCode::Success; }
    bool isNetworkError() const { return static_cast<uint32_t>(code) >= 0x0100 && static_cast<uint32_t>(code) < 0x0200; }
    bool isHttpError() const { return static_cast<uint32_t>(code) >= 0x0200 && static_cast<uint32_t>(code) < 0x0300; }
    bool isWsError() const { return static_cast<uint32_t>(code) >= 0x0300; }

    std::string toString() const {
        if (details.empty()) return message;
        return message + ": " + details;
    }

    static Error success() { return Error(); }
    static Error fromSystem(const std::error_code& ec);
    static Error make(ErrorCode code, const std::string& message, const std::string& details = "");
};

inline Error Error::make(ErrorCode code, const std::string& message, const std::string& details) {
    return Error(code, message, details);
}

inline Error Error::fromSystem(const std::error_code& ec) {
    ErrorCode code = ErrorCode::NetworkError;
    switch (ec.value()) {
        case 111: code = ErrorCode::ConnectionRefused; break;
        case 110: code = ErrorCode::ConnectionTimeout; break;
        case 104: code = ErrorCode::ConnectionReset; break;
        case 113: code = ErrorCode::HostUnreachable; break;
        case 32: code = ErrorCode::BrokenPipe; break;
        default: code = ErrorCode::NetworkError; break;
    }
    return Error(code, ec.message());
}

const char* errorCategoryName(ErrorCode code);

}
