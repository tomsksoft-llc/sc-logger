/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <scf/detail/to_string.h>
#include <scf/detail/type_pack.h>

namespace scf::detail {

/**
* The function is end point of the format processing
* (when the all arguments have put to the result string).
*/
inline void FormatProcessing(TypePack<>, const std::string &, std::size_t) {
}

/**
* Processing the format string:
* - get the arg as string (by specifier);
* - put the string arg to the result string.
* @tparam Indexes - indexes of the already processed specifiers
* @tparam T - type of the current processing argument
* @tparam Types - types of the remaining arguments
* @param specifier_indexes - typepack of the specifiers indexes
* @param result - result string
* @param offset - index offset after the previous arguments inserting
* @param arg - current processing argument
* @param args - remaining arguments
*/
template<typename ...Indexes, typename T, typename ...Types>
inline void FormatProcessing(
    TypePack<Indexes...> specifier_indexes,
    std::string &result,
    std::size_t offset,
    T &&arg,
    Types &&...args) {
    constexpr auto spec_index = Head(specifier_indexes);
    constexpr auto index = decltype(spec_index)::type::index;
    constexpr auto specifier = decltype(spec_index)::type::specifier;
    const auto real_position = index + offset;

    // erase 'specifier_size' characters from the source format starting from 'real_position'
    result.erase(real_position, specifiers::specifier_size);
    const auto arg_as_string = ToString<specifier>(arg);
    // put the argument as string to the result string starting from 'real_position'
    result.insert(real_position, arg_as_string);

    // calculate the index offset
    offset += arg_as_string.size() - specifiers::specifier_size;
    // continue inserting
    FormatProcessing(PopFront(specifier_indexes), result, offset, args...);
}

} // end of scf::detail

