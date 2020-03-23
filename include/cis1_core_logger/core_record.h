/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2020 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <optional>
#include <scl/record.h>
#include <scl/process_id.h>
#include <scl/detail/format_defines.h>

namespace cis1::core_logger {

// session_id is a string in the %Y-%m-%d-%H-%M-%S-pid_ppid format, where:
// %Y-%m-%d-%H-%M-%S contains time_length_k
// - is 1 char
// pid contains pid_length_k
// _ is 1 char
// ppid contains pid_length_k
const std::size_t session_id_length
    = scl::detail::log_formatting::time_length_k + 2 * scl::detail::log_formatting::pid_length_k + 2;

// the longest action name is startjob_stdout
const std::size_t action_length = 15;

/**
 * Log record info. The structure is set by logger and is handled by a recorder.
 */
class CoreRecord : public scl::IRecord {
public:
    explicit CoreRecord(scl::Level level_,
                        const std::string &time_str_,
                        const std::optional<std::string> &session_id_,
                        const std::optional<std::string> &action_,
                        const std::string &message_,
                        scl::ProcessId parent_pid_,
                        scl::ProcessId pid_);

protected:
    AlignedTokenCont AsAlignedTokens() const final;

    TokenCont AsTokens() const final;

    std::string Message() const final;

private:
    scl::Level level = scl::Level::Action;
    std::string time_str;
    std::optional<std::string> session_id;
    std::optional<std::string> action;
    std::string message;
    scl::ProcessId parent_pid = 0;
    scl::ProcessId pid = 0;
};

} // end of cis1::core_logger
