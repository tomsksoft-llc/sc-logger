/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2020 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <mutex>
#include <map>
#include <set>
#include <variant>

#include <cis1_webui_logger/protocol.h>
#include <scl/levels.h>
#include <scl/recorder.h>
#include <scl/process_id.h>
#include <scf/detail/type_matching.h>

namespace cis1::webui_logger {

class WebuiLogger;

/**
 * Non-moving logger pointer alias.
 */
using LoggerPtr = std::unique_ptr<WebuiLogger>;

/**
 * Logger implementation.
 * The main purpose of the logger are:
 *  - hold options (like align, available level and parent pid);
 *  - get log messages and other corresponding parameters;
 *  - move structured record message (as RecordInfo) to a recorder.
 */
class WebuiLogger {
public:
    /**
     * Initialization error info.
     */
    enum class InitError : int {
        IncorrectLogLevel = 1,
        NoRecorders,
        UnallocatedRecorder,
    };

    /**
     * Initialization result: ether pointer to an initialized logger or an error info.
     */
    using InitResult = std::variant<LoggerPtr, InitError>;

    /**
     * Logger options.
     */
    struct Options {
        /**
         * Logging messages which are less severe than level will be ignored.
         */
        scl::Level level = scl::Level::Action;
    };

    static std::string ToStr(InitError err) {
        switch (err) {
            case InitError::IncorrectLogLevel :
                return "IncorrectLogLevel";
            case InitError::NoRecorders :
                return "NoRecorders";
            case InitError::UnallocatedRecorder :
                return "UnallocatedRecorder";
            default:
                return "Unknown";
        }
    }

    /**
     * Init a Logger instance.
     * @param options - logger options
     * @param recorders - recorders that will handle log records
     * @return - ether pointer to an initialized logger or an error info
     */
    static InitResult Init(const Options &options, scl::RecordersCont &&recorders);

    /**
     * Default dtor.
     */
    ~WebuiLogger() = default;

    /**
     * Record a system message.
     * @param level - level of the record
     *                (if the value greater than an options.level, then the message will be skipped)
     * @param message - record message
     */
    void SysRecord(scl::Level level, const std::string &message);

    /**
     * Record a message.
     * @param level - level of the record
     *                (if the value greater than an options.level, then the message will be skipped)
     * @param handler - request handler name
     * @param protocol - protocol name
     * @param message - record message
     */
    void Record(scl::Level level, Protocol protocol, const std::string &handler, const std::string &message);

    /**
     * Record a message with user's info.
     * @param level - level of the record
     *                (if the value greater than an options.level, then the message will be skipped)
     * @param handler - request handler name
     * @param protocol - protocol name
     * @param remote_addr - remote address as string
     * @param email - user name
     * @param message - record message
     */
    void UserRecord(scl::Level level,
                    Protocol protocol,
                    const std::string &handler,
                    const std::string &remote_addr,
                    const std::string &email,
                    const std::string &message);

private:

    /**
     * Private constructor.
     * @param options - logger options.
     * @param recorder - recorder that will handle log records
     */
    explicit WebuiLogger(const Options &options, scl::RecordersCont &&recorder);

    /**
     * Message record implementation: record a message with the optional session id and action.
     * @param level - level of the record
     *                (if the value greater than an options.level, then the message will be skipped)
     * @param action - action
     * @param message - record message
     */
    void RecordImpl(scl::Level level,
                    const std::optional<Protocol> &protocol,
                    const std::optional<std::string> &handler,
                    const std::optional<std::string> &remote_addr,
                    const std::optional<std::string> &email,
                    const std::string &message);

    /**
     * Logger options.
     */
    Options m_options;

    /**
     * Recorder that processes log records (eg FileRecorder, ConsoleRecorder and other custom recorders)
     */
    scl::RecordersCont m_recorders;
};

} // end of cis1::webui_logger
