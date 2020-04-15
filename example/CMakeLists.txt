project(example)

if (NOT BUILD_EXAMPLE)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)
endif ()

add_executable(cis1_core_logger src/cis1_core_logger.cpp)
add_executable(cis1_webui_logger src/cis1_webui_logger.cpp)

target_link_libraries(cis1_core_logger sc_logger)
target_link_libraries(cis1_webui_logger sc_logger)

set_property(TARGET cis1_core_logger PROPERTY CXX_STANDARD 17)
set_property(TARGET cis1_core_logger PROPERTY CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -fno-exceptions")

set_property(TARGET cis1_webui_logger PROPERTY CXX_STANDARD 17)
set_property(TARGET cis1_webui_logger PROPERTY CMAKE_CXX_FLAGS "-pedantic -Wall -Wextra -fno-exceptions")
