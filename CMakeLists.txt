cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD  17)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)

project(LeichtDB
        VERSION 0.0.1
        DESCRIPTION "A simple basic Relational Database Management System (Educational) @ "
        LANGUAGES C CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(leichtdb_lib
    src/storage/disk/disk_manager.cpp
)
target_include_directories(leichtdb_lib PUBLIC src/include)

add_executable(leichtdb
        src/main.cpp
)
target_link_libraries(leichtdb PRIVATE leichtdb_lib)


enable_testing()

include(FetchContent)
FetchContent_Declare(googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.18.0.tar.gz
)
FetchContent_MakeAvailable(googletest)

add_executable(tests
        test/storage/disk/disk_manager_test.cpp
)

target_link_libraries(tests PRIVATE leichtdb_lib GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)