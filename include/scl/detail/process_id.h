/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

namespace scl::detail {

#if defined(__linux__) || defined(__APPLE__)
#include <zconf.h>

using ProcessId = pid_t;

inline unsigned int CurrentProcessId() {
	return static_cast<unsigned int>(getpid());
}
#elif defined(_WIN32) || defined(_WIN64)
#include <Windows.h>

using ProcessId = DWORD;

inline unsigned int CurrentProcessId() {
	return static_cast<unsigned int>(GetCurrentProcessId());
}

#else
inline unsigned int CurrentProcessId() {
    static_assert(false, "CurrentProcessId() called on an unknown OS");
}
#endif

} // end of scl::detail
