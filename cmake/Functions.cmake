################################################################################
# Functions
################################################################################

################################################################################
# Print all CMake environment variables to console
################################################################################
function(cmake_list_all_variables)
    MESSAGE("+++> -------------------- VARIABLES ------------------- ")
    get_cmake_property(_variableNames VARIABLES)
    list (REMOVE_DUPLICATES _variableNames)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message("+++> ${_variableName}=${${_variableName}}")
    endforeach()
    MESSAGE("+++> -------------------------------------------------- ")
endfunction()
