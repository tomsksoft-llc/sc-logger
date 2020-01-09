/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <memory>
#include <iostream>
#include <scl/align_info.h>
#include <scl/recorder.h>

namespace scl {

class ConsoleRecorder;

using ConsoleRecorderPtr = std::unique_ptr<ConsoleRecorder>;

class ConsoleRecorder : public IRecorder {
public:
    struct Options {
        /**
         * Allow to align entry attributes, if the values is set.
         */
        std::optional<AlignInfo> align_info = std::nullopt;
    };

    ~ConsoleRecorder() final = default;

    static ConsoleRecorderPtr Init(const Options &options) {
        return ConsoleRecorderPtr(new ConsoleRecorder(options));
    }

    inline void OnRecord(const RecordInfo &record) final {
        if (m_options.align_info) {
            std::cout << ToString(record, *m_options.align_info) << std::endl;
        } else {
            std::cout << ToString(record) << std::endl;
        }
    }

private:
    explicit ConsoleRecorder(const Options &options)
        : m_options(options) {
    }

    Options m_options;
};

}
