cmake_minimum_required(VERSION 3.5)
project(tests)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

enable_testing()
find_package(GTest REQUIRED)

add_executable(tests src/scf_test.cpp)

target_link_libraries(tests sc_logger GTest::GTest GTest::Main)

set_property(TARGET tests PROPERTY CXX_STANDARD 17)
set_property(TARGET tests PROPERTY CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -fno-exceptions")

add_test(NAME tests
         WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
         COMMAND tests
         )

