#include <variant>

#include <scl/logger.h>
#include <scl/record_info.h>
#include <scl/detail/process_id.h>
#include <scl/detail/misc.h>

namespace scl {

Logger::InitResult Logger::Init(const Options &options, RecordersCont &&recorders) {
    using Error = InitError;

    if (!detail::IsLevelCorrect(options.level)) {
        return Error::IncorrectLogLevel;
    }

    if (recorders.empty()) {
        return Error::NoRecorders;
    }

    for (const auto &recorder : recorders) {
        if (!recorder) {
            return Error::UnallocatedRecorder;
        }
    }

    return LoggerPtr(new Logger(options, std::move(recorders)));
}

void Logger::Record(Level level, const std::string &message) {
    const auto action = std::nullopt;
    RecordImpl(level, action, message);
}

Logger::Logger(const Logger::Options &options, RecordersCont &&recorder)
    : m_options(options),
      m_recorders(std::move(recorder)) {
}

void Logger::RecordImpl(Level level,
                        const std::optional<std::string> &action,
                        const std::string &message) {
    if (m_options.level < level) {
        // the level is not supported by settings
        return;
    }

    RecordInfo record_info{level,
                           CurTimeStr(),
                           m_options.session_id,
                           action,
                           message,
                           m_options.parent_pid,
                           detail::CurrentProcessId()};

    for (auto &recorder : m_recorders) {
        recorder->OnRecord(record_info);
    }
}

} // end of scl
