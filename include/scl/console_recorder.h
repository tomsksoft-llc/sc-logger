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
#include <scl/recorder.h>

namespace scl {

class ConsoleRecorder;

/**
 * Non-moving console recorder pointer alias.
 */
using ConsoleRecorderPtr = std::unique_ptr<ConsoleRecorder>;

/**
 * Console recorder that implement the IRecorder interface.
 */
class ConsoleRecorder : public IRecorder {
public:
    /**
     * Console recorder options.
     */
    struct Options {
        /**
         * Allow to align entry attributes, if the values is true.
         */
        bool align = false;
    };

    /**
     * Default derived dtor.
     */
    ~ConsoleRecorder() final = default;

    /**
     * Init a ConsoleRecorder instance.
     * @param options - console recorder options
     * @return - ether pointer to an initialized recorder or an error info
     */
    static ConsoleRecorderPtr Init(const Options &options) {
        return ConsoleRecorderPtr(new ConsoleRecorder(options));
    }

    /**
     * @overload
     */
    inline void OnRecord(const IRecord &record) final {
        if (m_options.align) {
            std::cout << record.ToAlignedString() << std::endl;
        } else {
            std::cout << record.ToString() << std::endl;
        }
    }

private:
    /**
     * Private ctor.
     * @param options - console recorder options
     */
    explicit ConsoleRecorder(const Options &options)
        : m_options(options) {
    }

    /**
     * Console recorder options.
     */
    Options m_options;
};

}
