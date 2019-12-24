/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <filesystem>
#include <optional>
#include <functional>

#include <scl/levels.h>
#include <scl/record_info.h>
#include <scl/detail/format_defines.h>

namespace scl {

namespace fs = std::filesystem;

/**
 * Attribute alignment info.
 * Other attributes like pid_length and time_length have constant length.
 */
class AlignInfo {
public:
    AlignInfo() = default;

    explicit AlignInfo(std::size_t _action_length, std::size_t _session_id_length)
        : action_length(_action_length),
          session_id_length(_session_id_length) {
    }

    [[nodiscard]]
    inline std::size_t ActionLength() const {
        return action_length;
    }

    [[nodiscard]]
    inline std::size_t SessionIdLength() const {
        return session_id_length;
    }

private:
    std::size_t action_length = detail::log_formatting::action_default_length_k;
    std::size_t session_id_length = detail::log_formatting::session_id_default_length_k;
};

class PathInfo {
public:
    PathInfo() = default;

    explicit PathInfo(fs::path _log_directory, std::string _file_name_template)
        : log_directory(std::move(_log_directory)),
          file_name_template(std::move(_file_name_template)) {
    }

    [[nodiscard]]
    inline const fs::path &LogDirectory() const {
        return log_directory;
    }

    [[nodiscard]]
    inline const std::string &FileNameTemplate() const {
        return file_name_template;
    }

private:
    /**
     * Path to a log directory.
     */
    fs::path log_directory;

    /**
     * File name template. The template is used to allow a log rotation.
     * The file_name_template value may contain the following specifiers:
     *   %t - current time (the specifier will be replaced by the filled "%Y-%m-%d-%H-%M-%S" template)
     *   %n - number of rotation iteration (start with '1')
     * 
     * If the size_limit value is set (!= std::nullopt), the file_name_template value must contain at least one of the above specifiers.
     */
    std::string file_name_template;
};

struct Options {
    using RecordCallback = std::function<void(const RecordInfo &)>;

    /**
     * Logging messages which are less severe than level will be ignored.
     */
    Level level = Level::Action;

    /**
     * Optional record callback.
     * The callback has to hold a function with the following signature:
     *   void(const RecordInfo &)
     * The callback is called on each record and can be used eg to transfer the stream of records to the other side.
     */
    std::optional<RecordCallback> callback;

    /**
     * Optional path to a log directory, file name template.
     * Allow to record to a file, if the value is set.
     */
    std::optional<PathInfo> path_info = std::nullopt;

    /**
     * Allow a log rotation, when a count of log file lines reaches a particular limit.
     */
    std::optional<std::size_t> size_limit = std::nullopt;

    /**
     * Allow to align entry attributes, if the values is set.
     */
    std::optional<AlignInfo> align_info = std::nullopt;

    /**
     * Use console output.
     */
    bool use_console_output = true;

    /**
     * Do not allow Logger to work, if all of the following conditions are true:
     * 1) the path_info is set;
     * 2) Logger couldn't open specified file.
     *
     * Note: Logger::Init() will return Result::CantOpenFile anyway,
     * even if all of the following conditions are true:
     * 1) the value and the use_console_output are false;
     * 2) Logger couldn't open a file.
     */
    bool exit_on_cant_open_file = true;

    /**
     * Parent process id
     */
    unsigned int parent_pid = 0;
};

}

