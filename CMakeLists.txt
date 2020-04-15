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
    "Build examples for self-check-logger library")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Filesystem REQUIRED Final)

add_library(
    sc_logger
    src/cis1_core_logger/core_logger.cpp
    src/cis1_core_logger/core_record.cpp
    src/cis1_webui_logger/webui_logger.cpp
    src/cis1_webui_logger/webui_record.cpp
    src/record.cpp)

target_include_directories(
    sc_logger
    PUBLIC
    include)

target_link_libraries(sc_logger
    PUBLIC
    std::filesystem)

set_property(TARGET sc_logger PROPERTY CXX_STANDARD 17)

if(BUILD_DOC)
    find_package(Doxygen REQUIRED)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(
        doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

if (BUILD_TESTING)
    add_subdirectory(test_package)
endif ()

if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif ()

install(
    TARGETS sc_logger
    LIBRARY
    DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(DIRECTORY "${CMAKE_SOURCE_DIR}/include/" # source directory
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.h" # select header files
)
