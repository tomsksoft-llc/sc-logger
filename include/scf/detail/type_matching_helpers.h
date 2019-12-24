/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

/**
* The file contains the special helpers that provide
* the ability to determine the match an argument type to a specifier
*/

#pragma once

#include <scf/detail/type_matching.h>
#include <scf/detail/specifiers.h>

namespace scf::detail {

/**
*
* @tparam C
* @tparam T
* @param val
* @return
*/
template<
    char C,
    typename T,
    std::enable_if_t<C == specifiers::string_spc_k> * = nullptr // C is 's' specifier
>
inline constexpr bool IsValMatchesSpecifierHelper(const T &val) {
  return IsString(val);
}

/**
*
* @tparam C
* @tparam T
* @param val
* @return
*/
template<
    char C,
    typename T,
    std::enable_if_t<C == specifiers::int_spc_k> * = nullptr // C is 'd' specifier
>
inline constexpr bool IsValMatchesSpecifierHelper(const T &val) {
  return IsInt(val);
}

/**
*
* @tparam C
* @tparam T
* @param val
* @return
*/
template<
    char C,
    typename T,
    std::enable_if_t<C == specifiers::char_spc_k> * = nullptr // C is 'c' specifier
>
inline constexpr bool IsValMatchesSpecifierHelper(const T &val) {
  return IsChar(val);
}

/**
*
* @tparam C
* @tparam T
* @param val
* @return
*/
template<
    char C,
    typename T,
    std::enable_if_t<C == specifiers::bool_spc_k> * = nullptr// C is 'b' specifier
>
inline constexpr bool IsValMatchesSpecifierHelper(const T &val) {
  return IsBool(val);
}

/**
*
* @tparam C
* @tparam T
* @param val
* @return
*/
template<
    char C,
    typename T,
    std::enable_if_t<C == specifiers::float_spc_k> * = nullptr // C is 'c' specifier
>
inline constexpr bool IsValMatchesSpecifierHelper(const T &val) {
  return IsFloat(val);
}

template<
    char C,
    typename T,
    std::enable_if_t<C == specifiers::user_type_spc_k> * = nullptr // C is 'U' specifier
>
inline constexpr bool IsValMatchesSpecifierHelper(const T &) {
    return IsThereToStringFor<T>::value;
}

} // end of scf::detail

