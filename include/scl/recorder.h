/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <memory>
#include <scl/record.h>

namespace scl {

/**
 * Recorder interface.
 * The interface maybe used to implement the required methods for a custom log recorder
 * different from standard FileRecorder or ConsoleRecorder.
 */
template<typename RecordT>
struct IRecorder {
    /**
     * Default virtual dtor.
     */
    virtual ~IRecorder() = default;

    /**
     * Handle a log record.
     * @param record - record info that should be handled.
     */
    virtual void OnRecord(const RecordT &record) = 0;
};

/**
 * Pointer of recorder interface.
 */
template<typename RecordT>
using RecorderPtr = std::unique_ptr<IRecorder<RecordT>>;

/**
 * Recorder pointers container.
 */
template<typename RecordT>
using RecordersCont = std::vector<RecorderPtr<RecordT>>;

} // end of scl
