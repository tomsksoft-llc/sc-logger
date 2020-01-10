/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <ctime>
#include <scl/detail/format_defines.h>

namespace scl {

/**
 * Get current time as string in the following format:
 * %Y-%m-%d-%H-%M-%S
 * @return time as string
 */
inline std::string CurTimeStr() {
    namespace Fmt = detail::log_formatting;

    std::time_t t = std::time(nullptr);
    // len(mbstr) = len(expected characters count) + '\0'
    char mbstr[detail::log_formatting::time_length_k + 1] = {0};

    std::strftime(mbstr, sizeof(mbstr), detail::log_formatting::time_format_k, std::localtime(&t));

    // even if the std::strftime returned 0, then the mbstr would be convert to the empty string
    // else the format is finished correctly
    return {mbstr};
}

}
