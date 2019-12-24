/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

/**
* The file contains the functions that provide
* the ability to determine the type of argument
*/

#pragma once

#include <string_view>

#include <scf/detail/type_pack.h>

namespace scf::detail {

#ifdef WEAK_TYPE_MATCHING

template <typename T>
inline constexpr bool IsString(const T &) {
  return std::is_convertible_v<T, std::string_view>
         || std::is_convertible_v<T, std::wstring_view>;
}

template <typename T>
inline constexpr bool IsInt(const T &) {
  using NoCvT = std::decay_t<T>;
  return std::is_integral_v<NoCvT>;
}

template <typename T>
inline constexpr bool IsChar(const T &val) {
  return IsInt(val);
}

template <typename T>
inline constexpr bool IsBool(const T &val) {
  return IsInt(val);
}

#else

inline constexpr bool IsStringImpl(const std::string &) {
    return true;
}

// use 'const' and '&' to make the argument constexpr
inline constexpr bool IsStringImpl(const std::string_view &) {
    return true;
}

template<typename ChatT, std::size_t N>
inline constexpr bool IsStringImpl(const ChatT (&)[N]) {
    using NoVolatileCharT = std::remove_volatile_t<ChatT>;
    return std::is_same_v<NoVolatileCharT, char>;
}

template<typename T>
inline constexpr bool IsStringImpl(const T &) {
    if constexpr (std::is_pointer_v<T>) {
        // there is no need to remove reference
        using NoCvPtrT = std::remove_cv_t<std::remove_pointer_t<T>>;
        return std::is_same_v<char, NoCvPtrT>;
    } else {
        return false;
    }
}

template<typename T>
inline constexpr bool IsString(const T &val) {
    return IsStringImpl(val);
}

template<typename T>
inline constexpr bool IsChar(const T &) {
    using NoCvT = std::decay_t<T>;
    using UChar = unsigned char;

    return Contains<NoCvT>(TypePack<char, UChar>{});
}

template<typename T>
inline constexpr bool IsBool(const T &) {
    using NoCvT = std::decay_t<T>;
    return std::is_same_v<NoCvT, bool>;
}

template<typename T>
inline constexpr bool IsInt(const T &) {
    using NoCvT = std::decay_t<T>;
    using UChar = unsigned char;
    using UShort = unsigned short;
    using UInt = unsigned int;
    using ULong = unsigned long;
    using UDLong = unsigned long long;

    return Contains<NoCvT>(
        TypePack<
            char, UChar, short, UShort, int,
            UInt, long, ULong, long long, UDLong>{});
}

#endif

template<typename T>
inline constexpr bool IsFloat(const T &) {
    using NoCvT = std::decay_t<T>;
    return std::is_floating_point_v<NoCvT>;
}

template<typename T>
class IsThereToStringFor {
private:
    template<typename S>
    static constexpr decltype(ToString(std::declval<S>())) detect(const S &) noexcept;

    static void detect(...) noexcept;

public:
    /**
     * Note: a ToSring() method must have return type = std::string
     */
    static constexpr bool value = std::is_same_v<std::string, decltype(detect(std::declval<T>()))>;
};

} // end of scf::detail

