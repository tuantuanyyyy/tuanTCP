cmake_minimum_required (VERSION 2.8.5)
cmake_policy (SET CMP0054 NEW)
project (tuanTCP)

include (etc/build_defs.cmake)
include (etc/build_type.cmake)
include (etc/cflags.cmake)

#include (etc/doxygen.cmake)

include (etc/clang_format.cmake)
include (etc/clang_tidy.cmake)
include (etc/cppcheck.cmake)

include_directories ("${PROJECT_SOURCE_DIR}/tuanlib/util")
include_directories ("${PROJECT_SOURCE_DIR}/tuanlib/tcp_helpers")
include_directories ("${PROJECT_SOURCE_DIR}/tuanlib")

add_subdirectory ("${PROJECT_SOURCE_DIR}/tuanlib")


add_subdirectory ("${PROJECT_SOURCE_DIR}/tests")



include (etc/tests.cmake)
