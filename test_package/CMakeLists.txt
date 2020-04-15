cmake_minimum_required(VERSION 3.5)
project(tests)

if (NOT BUILD_TESTING)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
endif()

enable_testing()

add_executable(tests src/scf_test.cpp src/scl_test.cpp)

if (NOT BUILD_TESTING)
    target_link_libraries(tests CONAN_PKG::sc_logger CONAN_PKG::gtest)
else ()
    target_link_libraries(tests sc_logger CONAN_PKG::gtest)
endif()

set_property(TARGET tests PROPERTY CXX_STANDARD 17)
set_property(TARGET tests PROPERTY CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -fno-exceptions")

add_test(NAME tests
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
         COMMAND tests
         )

