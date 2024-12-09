cmake_minimum_required(VERSION 3.31)

set(CMAKE_C_STANDARD                11)
set(CMAKE_CXX_STANDARD              17)
set(CMAKE_EXPORT_COMPILE_COMMANDS    1)

project(AOC24)

if(CMAKE_BUILD_TYPE MATCHES "Rel")
    message(STATUS "Release optimizations enabled")
    add_compile_options(-Ofast -flto)
    add_link_options(-flto)
else()
    message(STATUS "ASAN Enabled")
    add_compile_options(-fsanitize=address,undefined -g)
    add_link_options(-fsanitize=address,undefined -g)
endif()

if(DEFINED DEBUG)
    add_definitions(-DDEBUG)
endif()

if(DEFINED TRACE)
    add_definitions(-DTRACE)
endif()

include_directories(
  "${PROJECT_SOURCE_DIR}/inc/")

add_subdirectory(util)
link_libraries(
    arr
    file
    parse
    sort)

add_subdirectory(d01)
add_subdirectory(d02)
add_subdirectory(d03)
add_subdirectory(d04)
add_subdirectory(d05)
