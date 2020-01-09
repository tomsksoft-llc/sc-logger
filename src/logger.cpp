#include <variant>

#include <scl/logger.h>
#include <scl/record_info.h>
#include <scl/detail/process_id.h>
#include <scl/detail/misc.h>

namespace scl {

Logger::InitResult Logger::Init(const Options &options, RecorderPtr &&recorder) {
    using Error = InitError;

    if (!detail::IsLevelCorrect(options.level)) {
        return Error::IncorrectLogLevel;
    }

    if (!recorder) {
        return Error::EmptyRecorder;
    }

    return LoggerPtr(new Logger(options, std::move(recorder)));
}

void Logger::Record(Level level, const std::string &message) {
    const auto session_id = std::nullopt;
    const auto action = std::nullopt;
    RecordImpl(level, session_id, action, message);
}

void Logger::SesRecord(Level level,
                       const std::string &session_id,
                       const std::string &message) {
    const auto action = std::nullopt;
    RecordImpl(level, session_id, action, message);
}

Logger::Logger(const Logger::Options &options, RecorderPtr &&recorder)
    : m_options(options),
      m_recorder(std::move(recorder)) {
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

    m_recorder->OnRecord(record_info);
}

} // end of scl
