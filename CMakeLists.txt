cmake_minimum_required(VERSION 3.10)
project(ECS CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

include_directories(SYSTEM "${SOURCE_DIR}/ECS")

file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${SOURCE_DIR}/*.hpp")

add_executable(ecs ${SOURCE_FILES} ${HEADER_FILES})

find_package(glm REQUIRED)
include_directories(${GLM_INCLUDE_DIRS})