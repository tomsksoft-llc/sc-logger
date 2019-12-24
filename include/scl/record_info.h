/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <scl/levels.h>

namespace scl {

struct RecordInfo {
    scl::Level level = scl::Level::Action;
    std::string time;
    std::optional<std::string> session_id;
    std::optional<std::string> action;
    std::string message;
    unsigned int parent_pid = 0;
    unsigned int pid = 0;
};

} // end of scl
