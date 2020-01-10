/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <scf/detail/format_preprocessing.h>
#include <scf/detail/format_processing.h>

namespace scf::detail {

/**
* Start offset for FormatProcessing.
*/
const std::size_t start_offset_k = 0;

/**
* Start preprocessing index for FormatPreprocessing.
*/
const std::size_t start_preprocessing_index_k = 0;

/**
* The function is starting point for compile time format processing.
* @tparam StrHolder - type of the lambda function that contains the constexpr format string
* @tparam Types - types of the input arguments
* @param holder -lambda function that contains the constexpr format string
* @param args - input arguments
* @return - processed format
*/
template<typename StrHolder, typename ...Types>
inline std::string FormatImpl(StrHolder holder, Types &&... args) {
    constexpr std::size_t len = strlen(holder());

    // get the indexes of the specifiers in format string
    constexpr auto specifier_indexes
        = FormatPreprocessing<len, start_preprocessing_index_k>(holder, TypePack{}, args...);

    static_assert(Size(specifier_indexes) == sizeof...(Types), "unknown library error");

    std::string result = holder();
    // put the arguments to the result string
    FormatProcessing(specifier_indexes, result, start_offset_k, args...);
    return result;
}

} // end of scf::detail

