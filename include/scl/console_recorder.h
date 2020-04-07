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

template<typename RecordT>
class ConsoleRecorder;

/**
 * Alias of non-moving console recorder pointer.
 */
template<typename RecordT>
using ConsoleRecorderPtr = std::unique_ptr<ConsoleRecorder<RecordT>>;

/**
 * Console recorder that implement the IRecorder interface.
 */
template<typename RecordT>
class ConsoleRecorder : public IRecorder<RecordT> {
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
    static ConsoleRecorderPtr<RecordT> Init(const Options &options) {
        return ConsoleRecorderPtr<RecordT>(new ConsoleRecorder(options));
    }

    /**
     * @overload
     */
    inline void OnRecord(const RecordT &record) final {
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
