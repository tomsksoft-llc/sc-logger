#include <variant>

#include <cis1_core_logger/core_logger.h>
#include <cis1_core_logger/core_record.h>
#include <scl/detail/misc.h>

namespace cis1::core_logger {

CoreLogger::InitResult CoreLogger::Init(const Options &options, scl::RecordersCont &&recorders) {
    using Error = InitError;

    if (!scl::detail::IsLevelCorrect(options.level)) {
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

    return LoggerPtr(new CoreLogger(options, std::move(recorders)));
}

void CoreLogger::Record(scl::Level level, const std::string &message) {
    const auto session_id = std::nullopt;
    const auto action = std::nullopt;
    RecordImpl(level, session_id, action, message);
}

void CoreLogger::SesRecord(scl::Level level,
                           const std::string &message) {
    const auto action = std::nullopt;
    RecordImpl(level, m_options.session_id, action, message);
}

CoreLogger::CoreLogger(const CoreLogger::Options &options, scl::RecordersCont &&recorder)
    : m_options(options),
      m_recorders(std::move(recorder)) {
}

void CoreLogger::RecordImpl(scl::Level level,
                            const std::optional<std::string> &session_id,
                            const std::optional<std::string> &action,
                            const std::string &message) {
    if (m_options.level < level) {
        // the level is not supported by settings
        return;
    }

    CoreRecord record_info(level,
                           scl::CurTimeStr(),
                           session_id,
                           action,
                           message,
                           m_options.parent_pid,
                           m_options.pid);

    for (auto &recorder : m_recorders) {
        recorder->OnRecord(record_info);
    }
}

} // end of scl
