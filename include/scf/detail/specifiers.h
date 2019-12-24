/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

/**
* The file contains the specifier identifiers
*/

#pragma once

#include <cstddef>

namespace scf::detail::specifiers {

constexpr std::size_t specifier_size = 2;
constexpr char start_of_spec_subseq = '%';

constexpr char string_spc_k = 's';
constexpr char int_spc_k = 'd';
constexpr char char_spc_k = 'c';
constexpr char bool_spc_k = 'b';
constexpr char float_spc_k = 'f';
constexpr char user_type_spc_k = 'U';

} // end of scf::detail::specifiers

