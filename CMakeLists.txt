cmake_minimum_required(VERSION 3.25)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MIBot VERSION 1.0.0 LANGUAGES CXX)

add_library(${PROJECT_NAME} SHARED
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE src/)

if (NOT DEFINED ENV{GEODE_SDK})
    message(FATAL_ERROR "SDK not found")
else()
    message(STATUS "Found SDK: $ENV{GEODE_SDK}")
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

setup_geode_mod(${PROJECT_NAME})
