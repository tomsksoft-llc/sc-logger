#include <sstream>
#include <cis1_webui_logger/webui_record.h>

namespace cis1::webui_logger {

const std::size_t max_tokens_count = 6;

WebuiRecord::WebuiRecord(scl::Level level_,
                         const std::string &time_str_,
                         const std::string &message_,
                         const std::optional<Protocol> &protocol_,
                         const std::optional<std::string> &handler_,
                         const std::optional<std::string> &remote_addr_,
                         const std::optional<std::string> &email_)
    : level(level_),
      time_str(time_str_),
      message(message_),
      protocol(protocol_),
      handler(handler_),
      remote_addr(remote_addr_),
      email(email_) {
}

WebuiRecord::AlignedTokenCont WebuiRecord::AsAlignedTokens() const {
    namespace Fmt = scl::detail::log_formatting;

    AlignedTokenCont result;
    result.reserve(max_tokens_count);

    result.emplace_back(time_str, Fmt::time_length_k);
    result.emplace_back(scl::LevelToString(level), Fmt::level_length_k);

    if (protocol) {
        result.emplace_back(ProtocolToString(protocol.value()), protocol_length);
    }

    if (handler) {
        result.emplace_back(handler.value(), handler_length);
    }

    if (remote_addr) {
        result.emplace_back(remote_addr.value(), remote_addr_v4_length);
    }

    if (email) {
        const auto value = email.value();
        // do not align the email
        result.emplace_back(value, value.size());
    }

    return result;
}

WebuiRecord::TokenCont WebuiRecord::AsTokens() const {
    TokenCont result;
    result.reserve(max_tokens_count);

    result.emplace_back(time_str);
    result.emplace_back(scl::LevelToString(level));

    if (protocol) {
        result.emplace_back(ProtocolToString(protocol.value()));
    }

    if (handler) {
        result.emplace_back(handler.value());
    }

    if (remote_addr) {
        result.emplace_back(remote_addr.value());
    }

    if (email) {
        const auto value = email.value();
        // do not align the email
        result.emplace_back(value);
    }

    return result;
}

std::string WebuiRecord::Message() const {
    return message;
}

} // end of cis1::webui_logger
