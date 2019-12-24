#include <variant>
#include <ctime>
#include <iostream>

#include <scl/logger.h>
#include <scl/detail/format_defines.h>
#include <scl/detail/process_id.h>

namespace scl {

std::unique_ptr<Logger> Logger::Instance;

Logger::SpecifierPositions Logger::BreakDownTemplate(std::string_view templ,
                                                     const SpecifierSet &search_specifiers,
                                                     TemplateError &error) {
    using Error = TemplateError;

    const auto find_specifier_pos_fn
        = [&templ, &search_specifiers](std::size_t start_pos) -> std::size_t {
            // do not pass the pos value by reference, the function should new value
            const auto pos = templ.find(detail::file_name_formatting::start_of_spec_subseq_k, start_pos);
            if (pos == std::string::npos) {
                return std::string::npos;
            }

            return pos + 1 >= templ.size()
                   // the '%' is the last character of the file_name string
                   // therefore there is no need to replace the character
                   ? std::string::npos
                   // else return position to the '%' character
                   : pos;
        };

    SpecifierPositions specifier_positions;
    char specifier = 0;
    std::size_t pos = 0;

    while ((pos = find_specifier_pos_fn(pos)) != std::string::npos) {
        specifier = templ[pos + 1];
        if (!search_specifiers.count(specifier)) {
            error = Error::UnknownSpecifiers;
            return {};
        }

        if (specifier_positions.count(specifier)) {
            error = Error::DuplicateSpecifiers;
            return {};
        }

        specifier_positions[specifier] = pos;
        ++pos;
    }

    return specifier_positions;
}

std::string Logger::CurTimeStr() {
    std::time_t t = std::time(nullptr);
    // len(mbstr) = len(expected characters count) + '\0'
    char mbstr[detail::log_formatting::time_length_k + 1] = {0};

    std::strftime(mbstr, sizeof(mbstr), detail::log_formatting::time_format_k, std::localtime(&t));

    // even if the std::strftime returned 0, then the mbstr would be convert to the empty string
    // else the format is finished correctly
    return {mbstr};
}

InitResult Logger::Init(const Options &options) {
    using Result = InitResult;

    if (!detail::IsLevelCorrect(options.level)) {
        return Result::IncorrectLogLevel;
    }

    if (options.path_info) {
        const auto &path_info = *options.path_info;
        const auto &log_directory = path_info.LogDirectory();
        const auto &file_name_template = path_info.FileNameTemplate();

        if (log_directory.empty() || !fs::exists(log_directory)) {
            return Result::PathNotExists;
        }

        if (!fs::is_directory(log_directory)) {
            return Result::PathIsNotDirectory;
        }

        if (file_name_template.empty()) {
            return Result::EmptyFileNameTemplate;
        }

        TemplateError error = TemplateError::Ok;
        auto specifiers
            = BreakDownTemplate(file_name_template,
                                {
                                    detail::file_name_formatting::current_time_spc_k,
                                    detail::file_name_formatting::rotation_iteration_number_spc_k
                                },
                                error);

        if (error != TemplateError::Ok) {
            return Result::IncorrectFileNameTemplate;
        }

        Instance.reset(new Logger(options, std::move(specifiers)));

        // there is no need to lock a mutex,
        // because the function should be called once on an application start
        const auto open_file_result = Instance->OpenFile();
        if (open_file_result == OpenFileResult::CantOpenFile
            && (options.exit_on_cant_open_file
                || !options.use_console_output)) {
            Instance.reset();
            return Result::CantOpenFile;
        }

        return Result::Ok;
    }

    if (!options.use_console_output) {
        // path_info is not set and the use_console_output is false
        // therefore the logger has nothing to do
        return Result::NothingToDo;
    }

    Instance.reset(new Logger(options));
    return Result::Ok;
}

void Logger::Uninit() {
    AssertInstance();
    Instance.reset();
}

void Logger::Record(Level level, const std::string &message) {
    if (!Instance) {
        throw std::runtime_error("Logger instance is not initialized yet");
    }

    const auto session_id = std::nullopt;
    const auto action = std::nullopt;
    Instance->RecordImpl(level, session_id, action, message);
}

void Logger::SesRecord(Level level,
                       const std::string &session_id,
                       const std::string &message) {
    if (!Instance) {
        throw std::runtime_error("Logger instance is not initialized yet");
    }

    const auto action = std::nullopt;
    Instance->RecordImpl(level, session_id, action, message);
}

Logger::Logger(Options options)
    : m_options(std::move(options)) {
}

Logger::Logger(Options options, SpecifierPositions &&file_name_specifiers)
    : m_options(std::move(options)),
      m_file_name_specifiers(std::move(file_name_specifiers)) {
}

void Logger::RecordImpl(Level level,
                        const std::optional<std::string> &session_id,
                        const std::optional<std::string> &action,
                        const std::string &message) {
    if (m_options.level < level) {
        // the level is not supported by settings
        return;
    }

    RecordInfo record_info{level,
                           CurTimeStr(),
                           session_id,
                           action,
                           message,
                           m_options.parent_pid,
                           detail::CurrentProcessId()};

    const auto record = CompileRecord(record_info);
    if (m_options.use_console_output) {
        std::cout << record << std::endl;
    }

    if (m_options.callback) {
        const auto &callback = *m_options.callback;
        callback(record_info);
    }

    if (!m_options.path_info
        // if last time we couldn't open a file, don't try to do it again
        || !m_log_file.is_open()
        ) {
        // there is no need to check, open and write to file
        return;
    }

    // a log file is opened already

    // lock the mutex here, before the OpenFile() will be called
    const auto lock = LockMutex();
    const CheckFileSizeResult size_result = CheckFileSize(m_log_file_path, record.size());
    if (size_result != CheckFileSizeResult::Allowed
        // the file could be deleted or overflowed, try to open file again
        && OpenFile() != OpenFileResult::Ok
        ) {
        // couldn't open a log file
        return;
    }

    m_log_file << record << std::endl;
}

Logger::OpenFileResult Logger::OpenFile() {
    using Result = OpenFileResult;
    // if the method was called, the path_info should be set
    const auto &log_directory = m_options.path_info->LogDirectory();
    const auto &file_name_template = m_options.path_info->FileNameTemplate();
    fs::path log_file_path;
    CheckFileSizeResult file_size_result = CheckFileSizeResult::IsOverflowed;

    // true if the file name template contains a '%n' specifier
    const bool file_name_can_be_rotated
        = static_cast<bool>(m_file_name_specifiers.count(
            detail::file_name_formatting::rotation_iteration_number_spc_k));

    m_log_file.close();
    m_log_file_path.clear();

    do {
        // number of rotation iteration should start with '1'
        ++m_rotation_iteration;

        log_file_path = CompileFullPath();
        file_size_result = CheckFileSize(log_file_path);
    } while (file_size_result == CheckFileSizeResult::IsOverflowed
             && file_name_can_be_rotated);

    if (file_size_result == CheckFileSizeResult::IsOverflowed) {
        // file name template doesn't contain the "%n" specifier,
        // therefore we cannot change a file name by increasing the m_rotation_iteration
        return Result::CantOpenFile;
    }

    m_log_file.open(log_file_path, std::ios::app);
    if (!m_log_file.is_open()) {
        return Result::CantOpenFile;
    }

    m_log_file_path = log_file_path;
    return Result::Ok;
}

Logger::CheckFileSizeResult Logger::CheckFileSize(const fs::path &file,
                                                  std::size_t record_data_size /*=0*/) const {
    using Result = CheckFileSizeResult;

    std::error_code ec{};
    const auto size = fs::file_size(file, ec);
    if (ec) {
        // the file could be deleted or not created
        return Result::NotExists;
    }

    if (!m_options.size_limit) {
        // there is no need to rotate the log file
        return Result::Allowed;
    }

    const auto size_limit = *m_options.size_limit;
    if (size_limit <= size + record_data_size) {
        return Result::IsOverflowed;
    }

    return Result::Allowed;
}

fs::path Logger::CompileFullPath() const {
    // if the method was called, the path_info should be set
    std::string file_name = m_options.path_info->FileNameTemplate();

    std::size_t offset = 0;
    std::string dest_str;
    for (auto[specifier, pos] : m_file_name_specifiers) {
        if (specifier == detail::file_name_formatting::current_time_spc_k) {
            dest_str = CurTimeStr();
        } else if (specifier == detail::file_name_formatting::rotation_iteration_number_spc_k) {
            dest_str = std::to_string(m_rotation_iteration);
        } else {
            // something went wrong, please fix that
            throw std::runtime_error("internal error");
        }

        file_name.replace(pos + offset, detail::file_name_formatting::specifier_size, dest_str);
        offset += dest_str.size() - detail::file_name_formatting::specifier_size;
    }

    return m_options.path_info->LogDirectory() / file_name;
}

std::string Logger::CompileRecord(const RecordInfo &record) const {
    std::stringstream ss;
    const auto PutIntoStream
        = [&ss](std::string_view token, std::optional<std::size_t> align_length = std::nullopt) {
            if (!align_length) {
                ss << token << " | ";
                return;
            }

            const auto align_length_val = *align_length;
            const auto token_length
                = align_length_val < token.size()
                  ? align_length_val
                  : token.size();

            const auto difference = align_length_val - token_length;
            const auto left_pedding_length = difference / 2;
            const auto right_pedding_length = left_pedding_length + difference % 2;

            const std::string left_pedding(left_pedding_length, ' ');
            const std::string right_pedding(right_pedding_length, ' ');

            ss << left_pedding << token.substr(0, token_length) << right_pedding << " | ";
        };

    if (m_options.align_info) {
        PutIntoStream(CurTimeStr(), detail::log_formatting::time_length_k);
        PutIntoStream(std::to_string(m_options.parent_pid), detail::log_formatting::pid_length_k);
        PutIntoStream(std::to_string(detail::CurrentProcessId()), detail::log_formatting::pid_length_k);

        if (record.session_id) {
            PutIntoStream(*record.session_id, m_options.align_info->SessionIdLength());
        }

        if (record.action) {
            PutIntoStream(*record.action, m_options.align_info->ActionLength());
        }

        ss << record.message;
        return ss.str();
    }

    PutIntoStream(CurTimeStr());
    PutIntoStream(std::to_string(m_options.parent_pid));
    PutIntoStream(std::to_string(detail::CurrentProcessId()));

    if (record.session_id) {
        PutIntoStream(*record.session_id);
    }

    if (record.action) {
        PutIntoStream(*record.action);
    }

    ss << record.message;
    return ss.str();
}

std::optional<std::lock_guard<std::mutex>> Logger::LockMutex() {
#ifdef SCL_MULTITHREADED
    return std::make_optional<std::lock_guard>(m_record_mutex);
#endif
    return std::nullopt;
}

} // end of scl
