cmake_minimum_required(VERSION 3.14)
project(DungeonEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

include_directories(${googletest_SOURCE_DIR}/googletest/include)
include_directories(${googletest_SOURCE_DIR}/googlemock/include)
include_directories(includes)

file(GLOB SOURCE_FILES "src/*.cpp")
add_library(dungeon_lib STATIC ${SOURCE_FILES})

add_executable(dungeon_editor src/main.cpp)
target_link_libraries(dungeon_editor PRIVATE dungeon_lib)

enable_testing()

add_executable(run_tests tests/test_balfate.cpp)
target_link_libraries(run_tests PRIVATE dungeon_lib gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(run_tests)