# CSE 3206 - Lab 3 - Group 7 : Iterator & Mediator (CampusConnect)
cmake_minimum_required(VERSION 3.14)
project(CampusConnectPatterns VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W4 /permissive-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wshadow)
endif()

enable_testing()

# Each pattern is an independent module, so the project builds even when
# only some modules have been pushed to GitHub yet.
foreach(module iterator mediator tests)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${module}/CMakeLists.txt")
        add_subdirectory(${module})
    endif()
endforeach()
