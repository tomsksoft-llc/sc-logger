project(example)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(example src/main.cpp)

target_link_libraries(example sc_logger)

set_property(TARGET example PROPERTY CXX_STANDARD 17)
set_property(TARGET example PROPERTY CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -fno-exceptions")

