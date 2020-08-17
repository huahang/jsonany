cmake_minimum_required(VERSION 3.14)

set(CMAKE_TOOLCHAIN_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(any)

find_package(RapidJSON CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 14)

add_library(jsonany
  json/any.cpp
  json/any.h)

target_include_directories(jsonany PRIVATE ${RAPIDJSON_INCLUDE_DIRS})

add_executable(any
  main.cpp)

target_include_directories(any PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(any PRIVATE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(any PRIVATE jsonany)

enable_testing()

find_package(GTest CONFIG REQUIRED)

add_executable(jsonany_test
  jsonany_test.cpp)

target_include_directories(jsonany_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(jsonany_test PRIVATE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(jsonany_test PRIVATE jsonany)
target_link_libraries(jsonany_test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(JsonAnyTest jsonany_test)
