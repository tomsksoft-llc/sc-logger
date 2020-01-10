/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

/**
* The header contains the function that convert arguments to string using specifiers
*/

#pragma once

#include <sstream>

#include <scf/detail/specifiers.h>
#include <scf/detail/type_matching.h>

namespace scf::detail {

template<
    char Specifier,
    typename T,
    std::enable_if_t<Specifier == specifiers::string_spc_k> * = nullptr
>
inline std::string ToString(const T &arg) {
    return {arg};
}

template<
    char Specifier,
    std::enable_if_t<Specifier == specifiers::string_spc_k> * = nullptr
>
inline std::string ToString(std::string_view arg) {
    return arg.data();
}

template<
    char Specifier,
    std::enable_if_t<Specifier == specifiers::char_spc_k> * = nullptr
>
inline std::string ToString(char arg) {
    return {arg};
}

template<
    char Specifier,
    typename T,
    std::enable_if_t<
        Specifier == specifiers::int_spc_k
        || Specifier == specifiers::float_spc_k> * = nullptr
>
inline std::string ToString(T arg) {
    return std::to_string(arg);
}

template<
    char Specifier,
    typename T,
    std::enable_if_t<
        Specifier == specifiers::hex_spc_k> * = nullptr
>
inline std::string ToString(T arg) {
    std::stringstream ss;
    ss << "0x" << std::uppercase << std::hex;
    if constexpr (IsChar(arg)) {
        const auto unsigned_arg = static_cast<std::make_unsigned_t<T>>(arg);
        const auto integral_arg = static_cast<unsigned short>(unsigned_arg);
        ss << integral_arg;
    } else {
        ss << arg;
    }

    return ss.str();
}

template<
    char Specifier,
    std::enable_if_t<Specifier == specifiers::bool_spc_k> * = nullptr
>
inline std::string ToString(bool arg) {
    return arg ? "true" : "false";
}

template<
    char Specifier,
    typename T,
    std::enable_if_t<Specifier == specifiers::user_type_spc_k> * = nullptr
>
inline std::string ToString(const T &arg) {
    return ToString(arg);
}

} // end of scf::detail

