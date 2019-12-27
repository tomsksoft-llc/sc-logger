/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <memory>
#include <scl/record_info.h>

namespace scl {

struct IRecordHandler {
    virtual ~IRecordHandler() = default;

    virtual void OnRecord(const RecordInfo &record) = 0;
};

using RecordHandlerPtr = std::unique_ptr<IRecordHandler>;

}
