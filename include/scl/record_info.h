/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <string>
#include <vector>
#include <optional>

#include <scl/levels.h>
#include <scl/align_info.h>

namespace scl {

struct RecordInfo {
    Level level = Level::Action;
    std::string time_str;
    std::optional<std::string> session_id;
    std::optional<std::string> action;
    std::string message;
    unsigned int parent_pid = 0;
    unsigned int pid = 0;
};

std::string ToString(const RecordInfo &record, const AlignInfo &align_info);

std::string ToString(const RecordInfo &record);

} // end of scl
