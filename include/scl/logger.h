/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <mutex>
#include <map>
#include <set>
#include <variant>

#include <scl/levels.h>
#include <scl/recorder.h>
#include <scf/detail/type_matching.h>

namespace scl {

class Logger;

/**
 * Non-moving logger pointer alias.
 */
using LoggerPtr = std::unique_ptr<Logger>;

/**
 * Logger implementation.
 * The main purpose of the logger are:
 *  - hold options (like align, available level and parent pid);
 *  - get log messages and other corresponding parameters;
 *  - move structured record message (as RecordInfo) to a recorder.
 */
class Logger {
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
        Level level = Level::Action;

        /**
         * Current process id
         */
        ProcessId pid = 0;

        /**
         * Parent process id
         */
        ProcessId parent_pid = 0;

        /**
         * Optional session id
         */
        std::optional<std::string> session_id = std::nullopt;
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
    static InitResult Init(const Options &options, RecordersCont &&recorders);

    /**
     * Default dtor.
     */
    ~Logger() = default;

    /**
     * Record a message. Optional session id and action will not be put into a result log record.
     * @param level - level of the record
     *                (if the value greater than an options.level, then the message will be skipped)
     * @param message - record message
     */
    void Record(Level level, const std::string &message);

    /**
     * Record a message with the specified action. Optional session id will not be put into a result log record.
     * @tparam ActT - type of action
     * @param level - level of the record
     *                (if the value greater than an options.level, then the message will be skipped)
     * @param action - action
     * @param message - record message
     */
    template<typename ActT>
    inline void ActRecord(Level level,
                          const ActT &action,
                          const std::string &message) {
        RecordImpl(level, ActionAsString(action), message);
    }

private:
    /**
     * Convert an action to the string
     * @tparam ActT - type of action (must be string or there must be a ToString(ActT) function for the action)
     * @param action - action
     * @return - action as string
     */
    template<typename ActT>
    static std::string ActionAsString(const ActT &action) {
        constexpr bool is_there_to_string = scf::detail::IsThereToStringFor<ActT>::value;
        constexpr bool is_string = scf::detail::IsString(action);
        static_assert(is_there_to_string || is_string,
                      "there must be a ToString() function for the action");

        const auto session_id = std::nullopt;
        if constexpr (is_there_to_string) {
            return ToString(action);
        } else {
            return action;
        }
    }

    /**
     * Private constructor.
     * @param options - logger options.
     * @param recorder - recorder that will handle log records
     */
    explicit Logger(const Options &options, RecordersCont &&recorder);

    /**
     * Message record implementation: record a message with the optional session id and action.
     * @param level - level of the record
     *                (if the value greater than an options.level, then the message will be skipped)
     * @param action - action
     * @param message - record message
     */
    void RecordImpl(Level level,
                    const std::optional<std::string> &action,
                    const std::string &message);

    /**
     * Logger options.
     */
    Options m_options;

    /**
     * Recorder that processes log records (eg FileRecorder, ConsoleRecorder and other custom recorders)
     */
    RecordersCont m_recorders;
};

} // end of scl
