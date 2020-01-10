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

/**
 * Log record info. The structure is set by logger and is handled by a recorder.
 */
struct RecordInfo {
    Level level = Level::Action;
    std::string time_str;
    std::optional<std::string> session_id;
    std::optional<std::string> action;
    std::string message;
    unsigned int parent_pid = 0;
    unsigned int pid = 0;
};

/**
 * Serialize a record to an aligned string.
 * The format of serialized record:
 * time | parent_pid | pid | session_id | action | message
 * @param record - record info
 * @param align_info - align info
 * @return - serialized aligned record
 */
std::string ToString(const RecordInfo &record, const AlignInfo &align_info);

/**
 * Serialize a record to an non-aligned string.
 * The format of serialized record:
 * time | parent_pid | pid | session_id | action | message
 * @param record - record info
 * @return - serialized record
 */
std::string ToString(const RecordInfo &record);

} // end of scl
