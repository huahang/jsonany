cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake)

project(jsonany)

find_package(RapidJSON CONFIG REQUIRED)

set(CMAKE_CXX_STANDARD 14)

enable_testing()

find_package(GTest CONFIG REQUIRED)

add_executable(jsonany_test
  json/any.h
  test/jsonany_test.cpp
  test/test_structs.h
)

target_include_directories(jsonany_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(jsonany_test PRIVATE ${RAPIDJSON_INCLUDE_DIRS})

target_link_libraries(jsonany_test PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)

add_test(JsonAnyTest jsonany_test)
