/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <optional>

namespace scl {
/**
 * Log levels enumeration.
 */
enum class Level : int {
    Action = 0,
    Error,
    Info,
    Debug
};

inline std::optional<Level> LevelFromStr(std::string str) {
    for (auto &ch : str) {
        ch =::tolower(ch);
    }

    if (str == "action")    return Level::Action;
    if (str == "error")     return Level::Error;
    if (str == "info")      return Level::Info;
    if (str == "debug")     return Level::Debug;
    return std::nullopt;
}

inline std::string LevelToString(Level level) {
    switch(level) {
        case Level::Action:    return "Action";
        case Level::Error:     return "Error";
        case Level::Info:      return "Info";
        case Level::Debug:     return "Debug";
        default:               return "Unknown";
    }
}

namespace detail {
/**
 * Check if a level is correct.
 * @param level - level
 * @return true if the level corresponds log levels enumeration else false
 */
inline bool IsLevelCorrect(Level level) {
    switch (level) {
        case Level::Action:
        case Level::Error:
        case Level::Info:
        case Level::Debug: {
            return true;
        }

        default: {
            return false;
        }
    }
}
} // end of detail
} // end of scl

