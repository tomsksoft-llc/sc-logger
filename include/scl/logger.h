/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <memory>
#include <fstream>
#include <mutex>
#include <map>
#include <set>
#include <variant>

#include <scl/levels.h>
#include <scl/record_handler.h>
#include <scf/detail/type_matching.h>

namespace scl {

class Logger;

using LoggerPtr = std::unique_ptr<Logger>;

class Logger {
public:
    enum class InitError : int {
        IncorrectLogLevel = 1,
        EmptyRecordHandler,
    };

    using InitResult = std::variant<LoggerPtr, InitError>;

    struct Options {
        /**
         * Logging messages which are less severe than level will be ignored.
         */
        Level level = Level::Action;

        /**
         * Parent process id
         */
        unsigned int parent_pid = 0;
    };

    /**
     * Init a Logger singleton instance.
     */
    static InitResult Init(const Options &options, RecordHandlerPtr &&handler);

    void Record(Level level, const std::string &message);

    void SesRecord(Level level,
                   const std::string &session_id,
                   const std::string &message);

    template<typename ActT>
    inline void ActRecord(Level level,
                          const ActT &action,
                          const std::string &message) {
        const auto session_id = std::nullopt;
        RecordImpl(level, session_id, ActionAsString(action), message);
    }

    template<typename ActT>
    inline void SesActRecord(Level level,
                             const std::string &session_id,
                             const ActT &action,
                             const std::string &message) {
        RecordImpl(level, session_id, ActionAsString(action), message);
    }

private:
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
     */
    explicit Logger(const Options &options, RecordHandlerPtr &&handler);

    void RecordImpl(Level level,
                    const std::optional<std::string> &session_id,
                    const std::optional<std::string> &action,
                    const std::string &message);

    /**
     * Logger options.
     */
    Options m_options;

    RecordHandlerPtr m_handler;
};

} // end of scl
