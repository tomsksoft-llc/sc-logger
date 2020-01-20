/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

namespace scl {

#if defined(__linux__) || defined(__APPLE__)

#include <cstdint>

using ProcessId = pid_t;

#elif defined(_WIN32) || defined(_WIN64)

#include <Windows.h>
using ProcessId = DWORD;

#else

using ProcessId = void;

#endif

} // end of scl
