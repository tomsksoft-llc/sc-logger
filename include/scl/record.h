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

#include <scl/levels.h>

namespace scl {

/**
 * Log record info. The structure is set by logger and is handled by a recorder.
 */
class IRecord {
public:
    struct AlignToken {
        AlignToken(const std::string &_token, std::size_t _align) : token(_token), align(_align) {}

        std::string token{};
        std::size_t align = 0;
    };

    using AlignedTokenCont = std::vector<AlignToken>;
    using TokenCont = std::vector<std::string>;

    /**
     * Serialize a record to an non-aligned string.
     * @return - serialized record
     */
    std::string ToString() const;

    /**
     * Serialize a record to an aligned string.
     * @return - serialized record
     */
    std::string ToAlignedString() const;

protected:
    static std::string CompileRecord(const AlignedTokenCont &aligned_tokens);

    static std::string CompileRecord(const TokenCont &tokens);

    virtual AlignedTokenCont AsAlignedTokens() const = 0;

    virtual TokenCont AsTokens() const = 0;

    virtual std::string Message() const = 0;
};

} // end of scl
