/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <scf/detail/specifier_index.h>
#include <scf/detail/type_matching_helpers.h>

namespace scf::detail {

/**
 * Check if the C is a specifier.
* @tparam C - checking character
* @return true if the C character is a specifier
*/
template<char C>
inline constexpr bool IsSpecifier() {
    return C == specifiers::string_spc_k
           || C == specifiers::int_spc_k
           || C == specifiers::hex_spc_k
           || C == specifiers::char_spc_k
           || C == specifiers::bool_spc_k
           || C == specifiers::float_spc_k
           || C == specifiers::user_type_spc_k;
}

/**
* Preprocess the remaining format string:
* check if there are no more specifiers (because there are no more arguments).
* @tparam N - format length
* @tparam I - index of the current processing character
* @tparam StringHolder - type of the lambda function that contains the constexpr format string
* @tparam Indexes - indexes of the already processed specifiers
* @param holder - lambda function that contains the constexpr format string
* @param specifier_indexes - typepack of the processed specifiers indexes
* @return - typepack of the processed specifiers indexes
*/
template<std::size_t N, std::size_t I, typename StringHolder, typename ...Indexes>
inline constexpr auto FormatPreprocessing(StringHolder holder, TypePack<Indexes...> specifier_indexes) {
    [[maybe_unused]]
    constexpr auto format = holder();

    if constexpr (I + 1 >= N) {
        // end of string reached.
        // stop the format processing and return result indexes
        return specifier_indexes;
    } else {
        if constexpr (format[I] == '%') {
            // format[I + 1] (second processing character) must not be a specifier
            static_assert(!IsSpecifier<format[I + 1]>(),
                          "the number of arguments is less than the number of specifiers");
        }

        // continue the processing from the next format symbol
        return FormatPreprocessing<N, I + 1>(holder, specifier_indexes);
    }
}

/**
* Preprocess the format string:
* - check if the arguments match the format specifiers;
* - find and return indexes of the specifier subsequences.
* @tparam N - format length
* @tparam I - index of the current processing character
* @tparam StringHolder - type of the lambda that contains the constexpr format string
* @tparam Indexes - indexes of the already processed specifiers
* @tparam T - type of the current processing argument
* @tparam Types - types of the remaining arguments
* @param holder - lambda function that contains the constexpr format string
* @param specifier_indexes - typepack of the processed specifiers indexes
* @param arg - current processing argument
* @param args - remaining arguments
* @return - typepack of the processed specifiers indexes
*/
template<
    std::size_t N,
    std::size_t I,
    typename StringHolder,
    typename...Indexes,
    typename T,
    typename ...Types
>
inline constexpr auto FormatPreprocessing(
    StringHolder holder,
    TypePack<Indexes...> specifier_indexes,
    T &&arg,
    Types &&...args) {
    constexpr auto is_inside_bounds = I + 1 < N;
    static_assert(is_inside_bounds, "the number of specifiers is less than the number of arguments");

    constexpr auto format = holder();
    if constexpr (!is_inside_bounds) {
        return TypePack<>{};
    } else if constexpr (format[I] == specifiers::start_of_spec_subseq
                         && IsSpecifier<format[I + 1]>()) {
        // format[I] (first processing character) is '%'
        // second processing character is specifier ('s', 'c' etc).
        // the input argument must matches the format[I + 1] (second processing character) specifier
        static_assert(IsValMatchesSpecifierHelper<format[I + 1]>(std::forward<T>(arg)),
                      "the arguments don't match the specifiers");

        // put index of the start_of_spec_subseq (start of the specifier subsequence)
        constexpr auto indexes_if_match = PushBack<SpecifierIndex<format[I + 1], I>>(specifier_indexes);

        // continue the processing from the I + 2 format symbol
        return FormatPreprocessing<N, I + 2>(holder, indexes_if_match, args...);
    } else {
        // the first processing character is not a start_of_spec_subseq
        // or the second processing character is not a specifier.
        // continue the processing from the next format symbol
        return FormatPreprocessing<N, I + 1>(holder, specifier_indexes, arg, args...);
    }
}

} // end of scf::detail

