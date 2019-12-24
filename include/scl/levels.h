/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

namespace scl {
enum class Level : int {
    Action = 0,
    Error,
    Info,
    Debug
};

namespace detail {
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

