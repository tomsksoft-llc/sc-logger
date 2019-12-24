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

#include <string>

#include <scf/detail/specifiers.h>

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

