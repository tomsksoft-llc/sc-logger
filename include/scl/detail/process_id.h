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

/**
 * Process id alias.
 */
using ProcessId = pid_t;

/**
 * Get current pid within POSIX systems.
 * @return - pid
 */
inline unsigned int CurrentProcessId() {
	return static_cast<unsigned int>(getpid());
}
#elif defined(_WIN32) || defined(_WIN64)
#include <Windows.h>

/**
 * Process id alias.
 */
using ProcessId = DWORD;

/**
 * Get current pid within Windows systems.
 */
inline unsigned int CurrentProcessId() {
	return static_cast<unsigned int>(GetCurrentProcessId());
}

#else
inline unsigned int CurrentProcessId() {
    static_assert(false, "CurrentProcessId() called on an unknown OS");
}
#endif

} // end of scl::detail
