cmake_minimum_required(VERSION 3.10)
project(ECS CXX)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

set(EXECUTABLE_NAME ecs)
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TINYOBJ_PATH external/tinyobjloader)

file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${SOURCE_DIR}/*.hpp")

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES} ${HEADER_FILES})

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

target_include_directories(${EXECUTABLE_NAME} PUBLIC
    ${SOURCE_DIR}/src
    ${TINYOBJ_PATH}
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

target_link_libraries(${EXECUTABLE_NAME} Vulkan::Vulkan)
target_link_libraries(${EXECUTABLE_NAME} glfw)
