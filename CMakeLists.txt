cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD  17)
set(CMAKE_CXX_STANDARD_REQUIRED  ON)

project(LeichtDB
        VERSION 2026.1
        DESCRIPTION "A simple basic Relational Database Management System (Educational) @ https://github.com/kurocifer/LeichtDB"
        LANGUAGES C CXX
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(leichtdb
        src/main.cpp
        src/storage/disk/disk_manager.cpp
)

target_include_directories(leichtdb PRIVATE src/include)