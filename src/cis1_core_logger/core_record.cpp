#include <sstream>
#include <cis1_core_logger/core_record.h>

namespace cis1::core_logger {

const std::size_t max_tokens_count = 7;

CoreRecord::CoreRecord(scl::Level level_,
                       const std::string &time_str_,
                       const std::optional<std::string> &session_id_,
                       const std::optional<std::string> &action_,
                       const std::string &message_,
                       scl::ProcessId parent_pid_,
                       scl::ProcessId pid_)
    : level(level_),
      time_str(time_str_),
      session_id(session_id_),
      action(action_),
      message(message_),
      parent_pid(parent_pid_),
      pid(pid_) {
}

CoreRecord::AlignedTokenCont CoreRecord::AsAlignedTokens() const {
    namespace Fmt = scl::detail::log_formatting;

    AlignedTokenCont result;
    result.reserve(max_tokens_count);

    result.emplace_back(time_str, Fmt::time_length_k);
    result.emplace_back(std::to_string(parent_pid), Fmt::pid_length_k);
    result.emplace_back(std::to_string(pid), Fmt::pid_length_k);

    if (session_id) {
        result.emplace_back(*session_id, session_id_length);
    }

    if (action) {
        result.emplace_back(*action, action_length);
    }

    return result;
}

CoreRecord::TokenCont CoreRecord::AsTokens() const {
    TokenCont result;
    result.reserve(max_tokens_count);

    result.push_back(time_str);
    result.emplace_back(std::to_string(parent_pid));
    result.emplace_back(std::to_string(pid));

    if (session_id) {
        result.emplace_back(*session_id);
    }

    if (action) {
        result.emplace_back(*action);
    }

    return result;
}

std::string CoreRecord::Message() const {
    return message;
}

}
