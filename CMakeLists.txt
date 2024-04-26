cmake_minimum_required(VERSION 3.10)
project(ECS CXX)

set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)


set(EXECUTABLE_NAME ecs)
set(SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(TINYOBJ_PATH external/tinyobjloader)
set(SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp")
file(GLOB_RECURSE HEADER_FILES "${SOURCE_DIR}/*.hpp")

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES} ${HEADER_FILES})

target_include_directories(${EXECUTABLE_NAME} PUBLIC
    ${SOURCE_DIR}/src
    ${TINYOBJ_PATH}
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
)

target_link_libraries(${EXECUTABLE_NAME} Vulkan::Vulkan)
target_link_libraries(${EXECUTABLE_NAME} glfw)


file(GLOB SHADER_VERT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.vert")
file(GLOB SHADER_FRAG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*.frag")
set(SHADER_SOURCES ${SHADER_VERT_SOURCES} ${SHADER_FRAG_SOURCES})

foreach(SHADER_SOURCES ${SHADER_SOURCES})
    get_filename_component(SHADER_NAME ${SHADER_SOURCES} NAME)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER_NAME}.spv
        COMMAND glslc ${SHADER_SOURCES} -o ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER_NAME}.spv
        DEPENDS ${SHADER_SOURCES}
        COMMENT "Compiling shader: ${SHADER_NAME}"
    )
    list(APPEND SHADER_SPV_FILES ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${SHADER_NAME}.spv)
endforeach()

add_custom_target(shaders DEPENDS ${SHADER_SPV_FILES})
add_dependencies(${EXECUTABLE_NAME} shaders)