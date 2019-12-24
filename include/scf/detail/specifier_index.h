/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <type_traits>

namespace scf::detail {

/**
* The structure that contains the information about specifier:
* @tparam Specifier - specifier identifier
* @tparam Index - index of the specifier
*/
template<char Specifier, std::size_t Index>
struct SpecifierIndex {
    static constexpr auto specifier = Specifier;
    static constexpr auto index = Index;
};

} // end of scf::detail

