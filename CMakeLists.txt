cmake_minimum_required (VERSION 3.1)
project(FDCL-SERIAL)

set (CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/libraries/eigen)

set(fdcl_serial_src
    src/serial.cpp
)
add_library(fdcl_serial STATIC ${fdcl_serial_src})

add_executable(test_fdcl_serial src/test_fdcl_serial.cpp)
target_compile_options(test_fdcl_serial
    PRIVATE -Wall -O3 -std=c++11
)
target_link_libraries(test_fdcl_serial
    fdcl_serial
)

