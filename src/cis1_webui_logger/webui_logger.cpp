#include <variant>

#include <cis1_webui_logger/webui_logger.h>
#include <cis1_webui_logger/webui_record.h>
#include <scl/process_id.h>
#include <scl/detail/misc.h>

namespace cis1::webui_logger {

WebuiLogger::InitResult WebuiLogger::Init(const Options &options, scl::RecordersCont<WebuiRecord> &&recorders) {
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

    return LoggerPtr(new WebuiLogger(options, std::move(recorders)));
}

void WebuiLogger::SysRecord(scl::Level level, const std::string &message) {
    const auto protocol = std::nullopt;
    const auto handler = std::nullopt;
    const auto remote_addr = std::nullopt;
    const auto email = std::nullopt;
    RecordImpl(level, protocol, handler, remote_addr, email, message);
}

void WebuiLogger::Record(scl::Level level, Protocol protocol, const std::string &handler, const std::string &message) {
    const auto remote_addr = std::nullopt;
    const auto email = std::nullopt;
    RecordImpl(level, protocol, handler, remote_addr, email, message);
}

void WebuiLogger::UserRecord(scl::Level level,
                             Protocol protocol,
                             const std::string &handler,
                             const std::string &remote_addr,
                             const std::string &email,
                             const std::string &message) {
    RecordImpl(level, protocol, handler, remote_addr, email, message);
}

WebuiLogger::WebuiLogger(const WebuiLogger::Options &options, scl::RecordersCont<WebuiRecord> &&recorder)
    : m_options(options),
      m_recorders(std::move(recorder)) {
}

void WebuiLogger::RecordImpl(scl::Level level,
                             const std::optional<Protocol> &protocol,
                             const std::optional<std::string> &handler,
                             const std::optional<std::string> &remote_addr,
                             const std::optional<std::string> &email,
                             const std::string &message) {
    if (m_options.level < level) {
        // the level is not supported by settings
        return;
    }

    WebuiRecord record_info(level,
                            scl::CurTimeStr(),
                            message,
                            protocol,
                            handler,
                            remote_addr,
                            email);

    for (auto &recorder : m_recorders) {
        recorder->OnRecord(record_info);
    }
}

} // end of scl
