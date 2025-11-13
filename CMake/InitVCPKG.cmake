# ###############################################################################
# VCPKG Initialization
# ###############################################################################

# ###############################################################################
# Guard section
# ###############################################################################
if(DEFINED VCPKG_INITIALIZED)
    return()
endif()

message(STATUS "${CMAKE_CURRENT_LIST_FILE}: Setting up vcpkg toolchain")

set(VCPKG_INITIALIZED 1)

# ###############################################################################
# Toolchain settings
# ###############################################################################
find_program(VCPKG_EXEC_PATH vcpkg)

if(NOT EXISTS "${VCPKG_EXEC_PATH}")
    message(FATAL_ERROR "\n"
        "VCPKG export exetutable was not found!\n"
        "Please, add VCPKG folder to System Paths and run bootstrap.\n"
    )
endif()

get_filename_component(VCPKG_LOCATION ${VCPKG_EXEC_PATH} DIRECTORY)
cmake_path(NORMAL_PATH VCPKG_LOCATION OUTPUT_VARIABLE VCPKG_LOCATION)

set(VCPKG_EXPORT_PATH "${VCPKG_LOCATION}" CACHE FILEPATH "Export path for last vcpkg export")

if(NOT EXISTS "${VCPKG_EXPORT_PATH}")
    message(FATAL_ERROR "\nVCPKG export folder '${VCPKG_EXPORT_PATH}' was not found!\n")
endif()

mark_as_advanced(FORCE VCPKG_EXPORT_PATH)

set(CMAKE_TOOLCHAIN_FILE "${VCPKG_EXPORT_PATH}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "Toolchain file (e.g., from vcpkg)")

if(NOT EXISTS "${CMAKE_TOOLCHAIN_FILE}")
    message(FATAL_ERROR "\nToolchain file '${CMAKE_TOOLCHAIN_FILE}' was not found!\n")
endif()

mark_as_advanced(FORCE CMAKE_TOOLCHAIN_FILE)

# if(NOT DEFINED VCPKG_INSTALLED_DIR)
#     if(EXISTS "${VCPKG_EXPORT_PATH}/installed")
#         set(VCPKG_INSTALLED_DIR "${VCPKG_EXPORT_PATH}/installed" CACHE PATH "Vcpkg installed directory")
#         set(VCPKG_INSTALLED_DIR_NAME "installed" CACHE STRING "Name of vcpkg installed directory")
#     elseif(EXISTS "${VCPKG_EXPORT_PATH}/vcpkg_installed")
#         set(VCPKG_INSTALLED_DIR "${VCPKG_EXPORT_PATH}/vcpkg_installed" CACHE PATH "Vcpkg installed directory")
#         set(VCPKG_INSTALLED_DIR_NAME "vcpkg_installed" CACHE STRING "Name of vcpkg installed directory")
#     else()
#         message(FATAL_ERROR "\nCannot determine vcpkg installed directory!\n")
#     endif()

#     mark_as_advanced(FORCE VCPKG_INSTALLED_DIR)
#     mark_as_advanced(FORCE VCPKG_INSTALLED_DIR_NAME)
# endif()

if(CMAKE_TOOLCHAIN_FILE MATCHES ".*vcpkg\.cmake")
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(VCPKG_TARGET_TRIPLET "x64-windows" CACHE STRING "Target triplet for vcpkg")
        mark_as_advanced(FORCE VCPKG_TARGET_TRIPLET)
        set(VCPKG_TARGET_TRIPLET_OPTIONS "x64-windows" "x64-windows")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "Target triplet for vcpkg")
        mark_as_advanced(FORCE VCPKG_TARGET_TRIPLET)
        set(VCPKG_TARGET_TRIPLET_OPTIONS "x64-linux")
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
        set(VCPKG_TARGET_TRIPLET "x64-osx" CACHE STRING "Target triplet for vcpkg")
        mark_as_advanced(FORCE VCPKG_TARGET_TRIPLET)
        set(VCPKG_TARGET_TRIPLET_OPTIONS "x64-osx")
    else()
        message(WARNING "${CMAKE_CURRENT_LIST_FILE}: Cannot determine suitable "
            "platform for vcpkg. We expect Windows, Linux, or OS X. "
            "Got '${CMAKE_HOST_SYSTEM_NAME}'.")
    endif()

    set_property(CACHE VCPKG_TARGET_TRIPLET PROPERTY STRINGS ${VCPKG_TARGET_TRIPLET_OPTIONS})
    list(FIND VCPKG_TARGET_TRIPLET_OPTIONS ${VCPKG_TARGET_TRIPLET} VCPKG_TRIPLET_OPTION_INDEX)

    if(${VCPKG_TRIPLET_OPTION_INDEX} EQUAL -1)
        message(WARNING "Using vcpkg triplet: '${VCPKG_TARGET_TRIPLET}', "
            "expected values are: ${VCPKG_TARGET_TRIPLET_OPTIONS}")
    endif()
endif()
