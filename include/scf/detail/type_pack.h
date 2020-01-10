/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

/**
* The file contains the template parameter pack
* that simplifies working with the variadic type pack
*/

#pragma once

#include <type_traits>

namespace scf::detail {

template<typename T>
struct BaseType {
    using type = T;
};

template<typename ...Types>
struct TypePack {
};

template<typename ...Types>
constexpr std::size_t Size(TypePack<Types...>) {
    return sizeof...(Types);
}

template<typename T, typename ...Types>
constexpr TypePack<Types..., T> PushBack(TypePack<Types...>) {
    return {};
}

template<typename T, typename ...Types>
constexpr TypePack<Types...> PopFront(TypePack<T, Types...>) {
    return {};
}

template<typename T, typename ...Types>
constexpr BaseType<T> Head(TypePack<T, Types...>) {
    return {};
}

template<typename TypeX, typename TypeY>
constexpr bool IsWeakSame() {
    using NoCvRefX = std::remove_cv_t<std::remove_reference_t<TypeX>>;
    using NoCvRefY = std::remove_cv_t<std::remove_reference_t<TypeY>>;

    return std::is_same_v<NoCvRefX, NoCvRefY>;
}

template<typename T, typename ...Types>
constexpr bool Contains(TypePack<Types...>) {
    return (IsWeakSame<T, Types>() || ...);
}

template<typename ...TypesX, typename ...TypesY>
constexpr bool operator==(TypePack<TypesX...>, TypePack<TypesY...>) {
    return false;
}

template<typename ...Types>
constexpr bool operator==(TypePack<Types...>, TypePack<Types...>) {
    return true;
}

} // end of scf::detail

