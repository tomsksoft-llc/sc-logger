/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

#include <cstring>

#include <scf/detail/format_impl.h>

/**
* The macro that wraps the str in a lambda function
* to process the str as constexpr value
*/
#define SCFormat(str, ...) \
::scf::detail::FormatImpl([](){return str;}, ##__VA_ARGS__)

