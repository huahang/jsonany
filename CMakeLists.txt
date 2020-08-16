cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(any)

find_package(RapidJSON CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 14)

add_executable(any
  main.cpp
  json/any.cpp
  json/any.h)

target_include_directories(any PRIVATE ${RAPIDJSON_INCLUDE_DIRS})
