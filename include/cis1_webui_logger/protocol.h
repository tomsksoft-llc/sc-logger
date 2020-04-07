/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2020 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

namespace cis1::webui_logger {

// the longest protocol name is "HTTP_POST"
const std::size_t protocol_length = 9;

enum class Protocol : unsigned short {
    HTTP_GET = 1,
    HTTP_POST,
    WS,
};

inline std::string ProtocolToString(Protocol p) {
    switch (p) {
        case Protocol::HTTP_GET:    return "HTTP GET";
        case Protocol::HTTP_POST:   return "HTTP POST";
        case Protocol::WS:          return "WS";
        default:                    return "UNKNOWN";
    }
}

} // end of cis1::webui_logger
