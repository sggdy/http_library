#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "error.hpp"

namespace cpphttp {

enum class WsOpCode : uint8_t {
    CONTINUATION = 0x0,
    TEXT = 0x1,
    BINARY = 0x2,
    CLOSE = 0x8,
    PING = 0x9,
    PONG = 0xA
};

struct WsFrame {
    bool fin = true;
    WsOpCode opCode = WsOpCode::TEXT;
    bool masked = false;
    uint64_t payloadLen = 0;
    uint8_t maskingKey[4] = {0};
    std::vector<uint8_t> payload;
};

class WsCodec {
public:
    static Error encodeFrame(const WsFrame& frame, std::vector<uint8_t>& output);
    static Error decodeFrame(const uint8_t* data, size_t len, WsFrame& frame, size_t& consumed);
};

}
