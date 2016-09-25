cmake_minimum_required (VERSION 3.1)
project (memory_resident_set)

#support c++ 11
set (CMAKE_CXX_STANDARD 11)

set (SOURCES mem_manage.cpp main.cpp)

add_executable(mem_test ${SOURCES})

