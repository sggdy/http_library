#include "cpphttp/core/codec/ws_codec.hpp"
#include <cstring>

namespace cpphttp {

Error WsCodec::encodeFrame(const WsFrame& frame, std::vector<uint8_t>& output) {
    uint8_t firstByte = 0x80 | static_cast<uint8_t>(frame.opCode);
    if (!frame.fin) firstByte &= 0x7F;
    output.push_back(firstByte);

    if (frame.payloadLen < 126) {
        output.push_back(static_cast<uint8_t>(0x80 | frame.payloadLen));
    } else if (frame.payloadLen < 65536) {
        output.push_back(0xFE);
        output.push_back((frame.payloadLen >> 8) & 0xFF);
        output.push_back(frame.payloadLen & 0xFF);
    } else {
        output.push_back(0xFF);
        for (int i = 7; i >= 0; --i) {
            output.push_back((frame.payloadLen >> (i * 8)) & 0xFF);
        }
    }

    uint8_t maskKey[4] = {0x00, 0x00, 0x00, 0x00};
    output.insert(output.end(), maskKey, maskKey + 4);

    output.insert(output.end(), frame.payload.begin(), frame.payload.end());
    return Error::success();
}

Error WsCodec::decodeFrame(const uint8_t* data, size_t len, WsFrame& frame, size_t& consumed) {
    if (len < 2) {
        consumed = 0;
        return Error::make(ErrorCode::WsInvalidFrame, "Incomplete frame header");
    }

    frame.fin = (data[0] & 0x80) != 0;
    frame.opCode = static_cast<WsOpCode>(data[0] & 0x0F);
    frame.masked = (data[1] & 0x80) != 0;

    uint64_t payloadLen = data[1] & 0x7F;
    size_t headerLen = 2;

    if (payloadLen == 126) {
        if (len < 4) { consumed = 0; return Error::make(ErrorCode::WsInvalidFrame, "Incomplete extended length"); }
        payloadLen = (data[2] << 8) | data[3];
        headerLen = 4;
    } else if (payloadLen == 127) {
        if (len < 10) { consumed = 0; return Error::make(ErrorCode::WsInvalidFrame, "Incomplete extended length"); }
        payloadLen = 0;
        for (int i = 0; i < 8; ++i) {
            payloadLen = (payloadLen << 8) | data[2 + i];
        }
        headerLen = 10;
    }

    if (frame.masked) {
        if (len < headerLen + 4) { consumed = 0; return Error::make(ErrorCode::WsInvalidFrame, "Incomplete masking key"); }
        std::memcpy(frame.maskingKey, data + headerLen, 4);
        headerLen += 4;
    }

    if (len < headerLen + payloadLen) {
        consumed = 0;
        return Error::make(ErrorCode::WsInvalidFrame, "Incomplete payload");
    }

    frame.payloadLen = payloadLen;
    frame.payload.assign(data + headerLen, data + headerLen + payloadLen);

    if (frame.masked) {
        for (size_t i = 0; i < frame.payload.size(); ++i) {
            frame.payload[i] ^= frame.maskingKey[i % 4];
        }
    }

    consumed = headerLen + payloadLen;
    return Error::success();
}

}
