################################################################################
# Helper functions
################################################################################
macro(find_and_replace variable match_regex replace_regex)
    get_cmake_property(all_var_names VARIABLES)
    string(REGEX MATCHALL "(^|;)${variable}[A-Za-z0-9_-]*" matched_vars "${all_var_names}")
    foreach(var_name ${matched_vars})
        string(REGEX REPLACE "${match_regex}" "${replace_regex}" ${var_name} "${${var_name}}")
    endforeach()
    unset(matched_vars)
    unset(all_var_names)
endmacro()

################################################################################
# Updates for MSVC
################################################################################
if(CMAKE_CXX_COMPILER MATCHES ".*cl.exe$")
    foreach(flag_var CMAKE_C_FLAGS CMAKE_CXX_FLAGS VCPKG_C_FLAGS VCPKG_CXX_FLAGS CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE)
        find_and_replace("${flag_var}" [[([ \\t\\r\\n]*)/ZI([ \\t\\r\\n]*)]] [[\\1/Zi\\2]])
        find_and_replace("${flag_var}" [[([ \\t\\r\\n]*)/Z7([ \\t\\r\\n]*)]] [[\\1/Zi\\2]])
        find_and_replace("${flag_var}" [[([ \\t\\r\\n]*)/Ob1([ \\t\\r\\n]*)]] [[\\1/Ob2\\2]])
    endforeach()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /STACK:10000000")
endif()

################################################################################
# Updates for GCC and Clang
################################################################################
# if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
#     foreach(flag_var CMAKE_C_FLAGS CMAKE_CXX_FLAGS VCPKG_C_FLAGS VCPKG_CXX_FLAGS CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELEASE)
#         get_cmake_property("${flag_var}" [[([ \\t\\r\\n]*)-O2([ \\t\\r\\n]*)]] [[\\1-O3\\2]])
#     endforeach()
# endif()
