#include "cpphttp/error.hpp"

namespace cpphttp {

const char* errorCategoryName(ErrorCode code) {
    switch (code) {
        case ErrorCode::Success: return "Success";
        case ErrorCode::Unknown: return "Unknown";
        case ErrorCode::OutOfMemory: return "OutOfMemory";
        case ErrorCode::NetworkError: return "NetworkError";
        case ErrorCode::ConnectionRefused: return "ConnectionRefused";
        case ErrorCode::ConnectionTimeout: return "ConnectionTimeout";
        case ErrorCode::ConnectionReset: return "ConnectionReset";
        case ErrorCode::HostUnreachable: return "HostUnreachable";
        case ErrorCode::BrokenPipe: return "BrokenPipe";
        case ErrorCode::HttpError: return "HttpError";
        case ErrorCode::BadRequest: return "BadRequest";
        case ErrorCode::NotFound: return "NotFound";
        case ErrorCode::MethodNotAllowed: return "MethodNotAllowed";
        case ErrorCode::InternalServerError: return "InternalServerError";
        case ErrorCode::WsError: return "WsError";
        case ErrorCode::WsHandshakeFailed: return "WsHandshakeFailed";
        case ErrorCode::WsInvalidFrame: return "WsInvalidFrame";
        case ErrorCode::WsProtocolError: return "WsProtocolError";
        case ErrorCode::WsMaskingError: return "WsMaskingError";
        default: return "Unknown";
    }
}

}
