cmake_minimum_required(VERSION 3.22)

option(VERBOSE_OUTPUT OFF)

set(PROJECT_NAME ray-tracing)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

include(InitVCPKG)
include(Functions)
include(ToolchainUpdate)
include(GitVersion)

# Git Version implementation for CMake
get_version_info("APP")

# Initialize Date and Time variables
init_time_and_date_variables()

project(${PROJECT_NAME} VERSION ${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}.${APP_VERSION_TWEAK} LANGUAGES CXX)

set(EXE_PLATFORM_KEYWORD "")

if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(EXE_PLATFORM_KEYWORD "")
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(EXE_PLATFORM_KEYWORD "MACOSX_BUNDLE")
endif()

add_subdirectory(RayTracingCPU)
add_subdirectory(RayTracingGPU)

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ray-tracing-gpu)
endif()

if(VERBOSE_OUTPUT)
    cmake_list_all_variables()
endif()
