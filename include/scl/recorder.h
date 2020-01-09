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

struct IRecorder {
    virtual ~IRecorder() = default;

    virtual void OnRecord(const RecordInfo &record) = 0;
};

using RecorderPtr = std::unique_ptr<IRecorder>;

}
