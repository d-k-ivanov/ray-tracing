cmake_minimum_required(VERSION 3.22)

option(VERBOSE_OUTPUT OFF)

set(PROJECT_NAME ray-tracing)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

include(InitVCPKG)
include(Functions)

project(${PROJECT_NAME})

set(EXE_PLATFORM_KEYWORD "")
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(EXE_PLATFORM_KEYWORD "")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(EXE_PLATFORM_KEYWORD "MACOSX_BUNDLE")
endif()

set(SOURCE_FILES
    ## Main
    src/main.cpp

    # src/SomeSourceFile.cpp
    # src/SomeHeaderFile.h

    ## Shaders
    # resources/shaders/SomeFragmentShader.frag
    # resources/shaders/VertexShader.vert
)
source_group("Sources" SOURCE_FILES REGULAR_EXPRESSION ".*\\.(h|cpp)")
source_group("Shaders" SOURCE_FILES REGULAR_EXPRESSION ".*\\.(glsl|frag|vert)")

find_package(assimp REQUIRED)
find_package(glad REQUIRED)
find_package(glfw3 REQUIRED)
find_package(imgui REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Stb REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(
    ${PROJECT_NAME}
        assimp::assimp
        glad::glad
        glfw
        imgui::imgui
        OpenGL::GL
)

target_include_directories(${PROJECT_NAME} PRIVATE ${Stb_INCLUDE_DIR})

if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/dummy")
    message(FATAL_ERROR "\"${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/dummy\" exists, remove it!")
endif()

add_custom_target(
    always_run_target ALL
    DEPENDS
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/dummy" # fake! ensure we run!
)

add_custom_command(
    OUTPUT
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/dummy" # fake! ensure we run!
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/resources"
        "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/resources"
)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
endif()

if(VERBOSE_OUTPUT)
    cmake_list_all_variables()
endif()
