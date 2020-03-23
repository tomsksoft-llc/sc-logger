/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2020 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <optional>
#include <cis1_webui_logger/protocol.h>
#include <scl/record.h>
#include <scl/process_id.h>
#include <scl/detail/format_defines.h>

namespace cis1::webui_logger {

// with a margin
const std::size_t handler_length = 40;

// the longest IPv4 address name is 255.255.255.255:65535
const std::size_t remote_addr_v4_length = 21;

/**
 * Log record info. The structure is set by logger and is handled by a recorder.
 */
class WebuiRecord : public scl::IRecord {
public:
    explicit WebuiRecord(scl::Level level_,
                         const std::string &time_str_,
                         const std::string &message_,
                         const std::optional<Protocol> &protocol_,
                         const std::optional<std::string> &handler_,
                         const std::optional<std::string> &remote_addr_,
                         const std::optional<std::string> &email_);

protected:
    /**
     * @overload
     * Note the email will not be alligned
     */
    AlignedTokenCont AsAlignedTokens() const final;

    TokenCont AsTokens() const final;

    std::string Message() const final;

private:
    scl::Level level = scl::Level::Action;
    std::string time_str;
    std::string message;
    std::optional<Protocol> protocol;
    std::optional<std::string> handler;
    std::optional<std::string> remote_addr;
    std::optional<std::string> email;
};

} // end of cis1::webui_logger
