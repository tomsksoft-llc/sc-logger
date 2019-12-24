/*
 *    TomskSoft SC_LOGGER
 *
 *   (c) 2019 TomskSoft LLC
 *   (c) Sergey Boyko [bso@tomsksoft.com]
 *
 */

#pragma once

namespace scl {
enum class InitResult : int {
    Ok = 0,
    PathNotExists,
    PathIsNotDirectory,
    EmptyFileNameTemplate,
    CantOpenFile,
    IncorrectLogLevel,
    NothingToDo,
    IncorrectFileNameTemplate,
};
} // end of scl

