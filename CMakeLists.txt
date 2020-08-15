cmake_minimum_required(VERSION 3.17.3)

set(CMAKE_TOOLCHAIN_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(any)

find_package(RapidJSON CONFIG REQUIRED)
find_path(BOOST_FUSION_INCLUDE_DIRS "boost/fusion/adapted.hpp")

set(CMAKE_CXX_STANDARD 20)

include_directories(.)

add_executable(any
  main.cpp
  json/any.cpp
  json/any.h)

target_include_directories(any PRIVATE ${RAPIDJSON_INCLUDE_DIRS})
target_include_directories(any PRIVATE ${BOOST_FUSION_INCLUDE_DIRS})
