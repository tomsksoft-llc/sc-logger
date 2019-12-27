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
#include <scl/record_handler.h>

namespace scl {

class ConsoleHandler;

using ConsoleHandlerPtr = std::unique_ptr<ConsoleHandler>;

class ConsoleHandler : public IRecordHandler {
public:
    struct Options {
        /**
         * Allow to align entry attributes, if the values is set.
         */
        std::optional<AlignInfo> align_info = std::nullopt;
    };

    ~ConsoleHandler() final = default;

    static ConsoleHandlerPtr Init(const Options &options) {
        return ConsoleHandlerPtr(new ConsoleHandler(options));
    }

    inline void OnRecord(const RecordInfo &record) final {
        if (m_options.align_info) {
            std::cout << ToString(record, *m_options.align_info) << std::endl;
        } else {
            std::cout << ToString(record) << std::endl;
        }
    }

private:
    explicit ConsoleHandler(const Options &options)
        : m_options(options) {
    }

    Options m_options;
};

}
