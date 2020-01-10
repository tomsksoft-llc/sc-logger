/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <scl/detail/format_defines.h>

namespace scl {

/**
 * Attribute alignment info.
 * Other attributes like pid_length and time_length have constant length.
 */
struct AlignInfo {
    /**
     * Length of an aligned action field.
     */
    std::size_t action_length = detail::log_formatting::action_default_length_k;

    /**
     * Length of an aligned session id field.
     */
    std::size_t session_id_length = detail::log_formatting::session_id_default_length_k;
};

}
