# ###############################################################################
# Set GitVersion
# ###############################################################################
function(get_version_info prefix)
    set(MAIN_BRANCH Development)
    set(RELEASE_BRANCH_PREFIX Release)

    set(${prefix}_VERSION_SUCCESS 0 PARENT_SCOPE)
    set(${prefix}_VERSION_MAJOR 0)
    set(${prefix}_VERSION_MINOR 0)
    set(${prefix}_VERSION_PATCH 0)
    set(${prefix}_VERSION_TWEAK 0)
    set(${prefix}_VERSION_BRANCH unknown)
    set(${prefix}_VERSION_FLAG unknown)
    set(${prefix}_VERSION_DISTANCE 0)
    set(${prefix}_VERSION_STRING 0.0.0-unknown)
    set(${prefix}_VERSION_ISDIRTY 0 PARENT_SCOPE)

    if("${${prefix}_CUSTOM_VERSION_MAJOR}" STREQUAL "")
        set(${prefix}_CUSTOM_VERSION_MAJOR 0)
    endif()

    if("${${prefix}_CUSTOM_VERSION_MINOR}" STREQUAL "")
        set(${prefix}_CUSTOM_VERSION_MINOR 0)
    endif()

    if("${${prefix}_CUSTOM_VERSION_PATCH}" STREQUAL "")
        set(${prefix}_CUSTOM_VERSION_PATCH 0)
    endif()

    if("${${prefix}_CUSTOM_VERSION_TWEAK}" STREQUAL "")
        set(${prefix}_CUSTOM_VERSION_TWEAK 0)
    endif()

    find_package(Git QUIET)

    if(GIT_FOUND)
        # Get the version info from the last tag (using the configured version tag prefix)
        execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --first-parent --match "[0-9].[0-9]*"
            RESULT_VARIABLE result
            OUTPUT_VARIABLE GIT_TAG_VERSION
            ERROR_VARIABLE error_out
            OUTPUT_STRIP_TRAILING_WHITESPACE
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )

        if(result EQUAL 0)
            # Extract version major, minor, patch from the result
            if(GIT_TAG_VERSION MATCHES "^?([0-9]+)\\.([0-9]+)(\\.([0-9]+))?(-([0-9]+))?.*$")
                set(${prefix}_VERSION_MAJOR ${CMAKE_MATCH_1})
                set(${prefix}_VERSION_MINOR ${CMAKE_MATCH_2})

                if(NOT ${CMAKE_MATCH_4} STREQUAL "")
                    set(${prefix}_VERSION_PATCH ${CMAKE_MATCH_4})
                endif()

                if(NOT ${CMAKE_MATCH_6} STREQUAL "")
                    set(${prefix}_VERSION_DISTANCE ${CMAKE_MATCH_6})
                endif()
            endif()
        else()
            # git describe return with error - just get the distance
            execute_process(COMMAND ${GIT_EXECUTABLE} rev-list --count HEAD
                RESULT_VARIABLE result
                OUTPUT_VARIABLE GIT_DISTANCE
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_VARIABLE error_out
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
            )

            if(result EQUAL 0)
                set(${prefix}_VERSION_DISTANCE ${GIT_DISTANCE})
            endif()
        endif()

        # Check for local modifications ...
        execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always --first-parent --dirty
            RESULT_VARIABLE result
            OUTPUT_VARIABLE GIT_ALWAYS_VERSION
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_VARIABLE error_out
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )

        if(result EQUAL 0)
            if(GIT_ALWAYS_VERSION MATCHES "^.*-dirty$")
                set(${prefix}_VERSION_ISDIRTY 1 PARENT_SCOPE)
            endif()
        endif()

        # Check the branch we are on
        execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
            RESULT_VARIABLE result
            OUTPUT_VARIABLE GIT_BRANCH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_VARIABLE error_out
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )

        if(result EQUAL 0)
            if("${GIT_BRANCH}" STREQUAL "HEAD" AND NOT "$ENV{FALLBACK_BRANCH}" STREQUAL "")
                set(GIT_BRANCH "$ENV{FALLBACK_BRANCH}")
            endif()

            set(${prefix}_VERSION_BRANCH "${GIT_BRANCH}")
            set(${prefix}_VERSION_BRANCH "${GIT_BRANCH}" PARENT_SCOPE)

            # Main branch (Developement)
            string(COMPARE EQUAL ${MAIN_BRANCH} ${GIT_BRANCH} ON_MAIN_BRANCH)

            # Release branches
            string(LENGTH ${RELEASE_BRANCH_PREFIX} PREFIX_LEN)
            string(SUBSTRING ${GIT_BRANCH} 0 ${PREFIX_LEN} COMPARE_PREFIX)
            string(COMPARE EQUAL ${RELEASE_BRANCH_PREFIX} ${COMPARE_PREFIX} ON_RELEASE_BRANCH)

            # Developer's branches
            if(NOT ON_RELEASE_BRANCH OR NOT ON_MAIN_BRANCH)
                set(ON_DEV_BRANCH TRUE)
                string(REGEX MATCH "^.+\/" DEV_BRANCH_PREFIX ${GIT_BRANCH})
                string(REGEX REPLACE "/+$" "" DEV_BRANCH_PREFIX "${DEV_BRANCH_PREFIX}")
            endif()

            if(ON_RELEASE_BRANCH)
                set(${prefix}_VERSION_FLAG release)
                set(RC_VERSION_MAJOR 0)
                set(RC_VERSION_MINOR 0)
                set(RC_VERSION_PATCH 0)

                # Check release branch name for version information (e.g. release/1.0)
                if(GIT_BRANCH MATCHES "^${RELEASE_BRANCH_PREFIX}.*([0-9]+)\\.([0-9]+)(\\.([0-9]+))?.*$")
                    set(RC_VERSION_MAJOR ${CMAKE_MATCH_1})

                    if(NOT ${CMAKE_MATCH_2} STREQUAL "")
                        set(RC_VERSION_MINOR ${CMAKE_MATCH_2})
                    endif()

                    if(NOT ${CMAKE_MATCH_4} STREQUAL "")
                        set(RC_VERSION_PATCH ${CMAKE_MATCH_4})
                    endif()
                endif()

                # If the release branch version is greater, use that version...
                if("${RC_VERSION_MAJOR}.${RC_VERSION_MINOR}.${RC_VERSION_PATCH}" VERSION_GREATER
                    "${${prefix}_VERSION_MAJOR}.${${prefix}_VERSION_MINOR}.${${prefix}_VERSION_PATCH}")
                    set(${prefix}_VERSION_MAJOR ${RC_VERSION_MAJOR})
                    set(${prefix}_VERSION_MINOR ${RC_VERSION_MINOR})
                    set(${prefix}_VERSION_PATCH ${RC_VERSION_PATCH})
                endif()

                # Try to get distance from last rc start tag
                execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --first-parent --match "[0-9].[0-9]*"
                    RESULT_VARIABLE result
                    OUTPUT_VARIABLE GIT_RC_TAG_VERSION
                    ERROR_VARIABLE error_out
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
                )

                if(result EQUAL 0)
                    if(GIT_RC_TAG_VERSION MATCHES "^?([0-9]+)\\.([0-9]+)(\\.([0-9]+))?(-([0-9]+))?.*$")
                        if(NOT ${CMAKE_MATCH_6} STREQUAL "")
                            set(${prefix}_VERSION_DISTANCE ${CMAKE_MATCH_6})
                        else()
                            set(${prefix}_VERSION_DISTANCE 0)
                        endif()
                    endif()
                endif()

            elseif(ON_MAIN_BRANCH)
                set(${prefix}_VERSION_FLAG "")
            elseif(ON_DEV_BRANCH)
                set(${prefix}_VERSION_FLAG ${DEV_BRANCH_PREFIX})
            endif()
        endif()

        set(${prefix}_VERSION_FLAG ${${prefix}_VERSION_FLAG} PARENT_SCOPE)
        set(${prefix}_VERSION_DISTANCE ${${prefix}_VERSION_DISTANCE} PARENT_SCOPE)

        execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
            RESULT_VARIABLE resultSH
            OUTPUT_VARIABLE GIT_SHORT_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_VARIABLE error_out
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )

        if(resultSH EQUAL 0)
            set(${prefix}_VERSION_SHORTHASH ${GIT_SHORT_HASH} PARENT_SCOPE)
        else()
            message(STATUS "Version-Info: Could not fetch short version hash.")
            set(${prefix}_VERSION_SHORTHASH "unknown" PARENT_SCOPE)
        endif()

        execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
            RESULT_VARIABLE resultFH
            OUTPUT_VARIABLE GIT_FULL_HASH
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_VARIABLE error_out
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        )

        if(resultFH EQUAL 0)
            set(${prefix}_VERSION_FULLHASH ${GIT_FULL_HASH} PARENT_SCOPE)
        else()
            message(STATUS "Version-Info: Could not fetch full version hash.")
            set(${prefix}_VERSION_FULLHASH "unknown" PARENT_SCOPE)
        endif()

        if(resultSH EQUAL 0 AND resultFH EQUAL 0)
            set(${prefix}_VERSION_SUCCESS 1 PARENT_SCOPE)
        endif()
    else() # Git not found ...
        message(STATUS "Version-Info: Git not found. Possible incomplete version information.")
    endif()

    if("${${prefix}_VERSION_BRANCH}" STREQUAL "unknown" OR "${${prefix}_VERSION_BRANCH}" STREQUAL "")
        if("${${prefix}_FALLBACK_VERSION_TYPE}" STREQUAL "release")
            set(ON_MAIN_BRANCH ON)
            set(${prefix}_VERSION_FLAG "")
            set(${prefix}_VERSION_FLAG "" PARENT_SCOPE)
        endif()

        set(${prefix}_VERSION_BRANCH "not-within-git-repo" PARENT_SCOPE)
    endif()

    # Check if overrule version is greater than dynamically created one
    if("${${prefix}_CUSTOM_VERSION_MAJOR}.${${prefix}_CUSTOM_VERSION_MINOR}.${${prefix}_CUSTOM_VERSION_PATCH}.${${prefix}_CUSTOM_VERSION_TWEAK}"
        VERSION_GREATER "${${prefix}_VERSION_MAJOR}.${${prefix}_VERSION_MINOR}.${${prefix}_VERSION_PATCH}.${${prefix}_VERSION_TWEAK}")
        set(${prefix}_VERSION_MAJOR ${${prefix}_CUSTOM_VERSION_MAJOR})
        set(${prefix}_VERSION_MINOR ${${prefix}_CUSTOM_VERSION_MINOR})
        set(${prefix}_VERSION_PATCH ${${prefix}_CUSTOM_VERSION_PATCH})
        set(${prefix}_VERSION_TWEAK ${${prefix}_CUSTOM_VERSION_TWEAK})
    endif()

    set(${prefix}_VERSION_MAJOR ${${prefix}_VERSION_MAJOR} PARENT_SCOPE)
    set(${prefix}_VERSION_MINOR ${${prefix}_VERSION_MINOR} PARENT_SCOPE)
    set(${prefix}_VERSION_PATCH ${${prefix}_VERSION_PATCH} PARENT_SCOPE)
    set(${prefix}_VERSION_DISTANCE ${${prefix}_VERSION_DISTANCE} PARENT_SCOPE)
    set(${prefix}_VERSION_TWEAK ${${prefix}_VERSION_DISTANCE} PARENT_SCOPE)

    # Build version string...
    set(VERSION_STRING "${${prefix}_VERSION_MAJOR}.${${prefix}_VERSION_MINOR}.${${prefix}_VERSION_PATCH}.${${prefix}_VERSION_DISTANCE}")

    if(NOT ON_MAIN_BRANCH)
        set(VERSION_STRING "${VERSION_STRING}-${${prefix}_VERSION_FLAG}")
    endif()

    set(${prefix}_VERSION_STRING "${VERSION_STRING}" PARENT_SCOPE)
endfunction()