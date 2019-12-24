cmake_minimum_required(VERSION 3.9)
project(sc_logger LANGUAGES CXX)

# Use 'add_definitions(-DSCL_MULTITHREADED)' if an application is multithreaded

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

set(BUILD_DOC
    ON
    CACHE BOOL
    "Build documentation")

set(BUILD_TESTING
    ON
    CACHE BOOL
    "Build unit tests for self-check-logger library")

set(BUILD_EXAMPLE
    OFF
    CACHE BOOL
    "Build examples for self-check-locckeg library")


find_package(Filesystem REQUIRED Final)
add_library(sc_logger src/logger.cpp)

target_include_directories(sc_logger
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

target_link_libraries(sc_logger
    PUBLIC
    std::filesystem
    ${CONAN_LIBS}
    )

set_property(TARGET sc_logger PROPERTY CXX_STANDARD 17)

if (BUILD_TESTING)
    add_subdirectory(test_package)
endif ()

if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif ()

