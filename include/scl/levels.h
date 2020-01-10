/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

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

